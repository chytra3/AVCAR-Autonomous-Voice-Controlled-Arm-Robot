import rclpy
from rclpy.node import Node
from std_msgs.msg import String

import speechmatics
from httpx import HTTPStatusError
import asyncio
import pyaudio
import os
import time
import google.generativeai as genai
import pyttsx3

API_KEY = "4WR5CeHahpGx9T0q4ftAZVGxzbBXD3f5"
LANGUAGE = "en"
CONNECTION_URL = f"wss://eu2.rt.speechmatics.com/v2/en"
CHUNK_SIZE = 1024
TRIGGER_WORD = "amigo"
TRANSCRIPTION_FILE = "transcription.txt"
TRANSCRIPTION_DURATION = 13  # seconds

class AudioProcessor:
    def __init__(self):
        self.wave_data = bytearray()
        self.read_offset = 0

    async def read(self, chunk_size):
        while self.read_offset + chunk_size > len(self.wave_data):
            await asyncio.sleep(0.001)
        new_offset = self.read_offset + chunk_size
        data = self.wave_data[self.read_offset:new_offset]
        self.read_offset = new_offset
        return data

    def write_audio(self, data):
        self.wave_data.extend(data)
        return

audio_processor = AudioProcessor()

# PyAudio callback
def stream_callback(in_data, frame_count, time_info, status):
    audio_processor.write_audio(in_data)
    return in_data, pyaudio.paContinue

def pddl():
    genai.configure(api_key="AIzaSyDMCkPgiZCxhpZAlCNia4dwwoFEt4f8sTk")

    # Set up the model
    generation_config = {
        "temperature": 1,
        "top_p": 0.95,
        "top_k": 64,
        "max_output_tokens": 8192,
    }

    safety_settings = [
        {
            "category": "HARM_CATEGORY_HARASSMENT",
            "threshold": "BLOCK_MEDIUM_AND_ABOVE"
        },
        {
            "category": "HARM_CATEGORY_HATE_SPEECH",
            "threshold": "BLOCK_MEDIUM_AND_ABOVE"
        },
        {
            "category": "HARM_CATEGORY_SEXUALLY_EXPLICIT",
            "threshold": "BLOCK_MEDIUM_AND_ABOVE"
        },
        {
            "category": "HARM_CATEGORY_DANGEROUS_CONTENT",
            "threshold": "BLOCK_MEDIUM_AND_ABOVE"
        },
    ]

    model = genai.GenerativeModel(model_name="gemini-1.5-pro",
                              generation_config=generation_config,
                              safety_settings=safety_settings)

    with open("transcription.txt", "r") as file:
        voice_command = file.read()

    prompt_parts = [
        "using Natural language processing identify objects, postions from the input string there are 3 objects (coke_can,can,cola,bowl,beer,syrup,medicine) positions can be pos1,pos2,etc....\nin case if there is no object from above three object return \"Invalid\"and if inappropriate input then return \"invalid\"\n\nhere you also have to consider cola=coke_can=can (all three object are the same)\nsimilarly medicine=beer=syrup(these three are same)\n\nyou are suppose to pharse the object and to postion from the input and print the pddl syntax\n\nonly print pddl syntax nothing else the response should contain only  pddl syntax\nit can start with move, pick, place \nexample : - can you pick coke_can and place it in pos2\noutput expected : - (and(located_at coke_can pos2))\n\nexample : - can you pick table and place it in pos2\noutput expected : - invalid (because table was not an object at all)\nexample:- pick coke_can and place it in pos5  place bowl to pos 6 move beer to pos4\n\noutput :- (and (located_at coke_can pos5) (located_at bowl pos6) (located_at beer pos4) )\n\nNote do not print python program\n\nexample : - pick coke_can and place it in pos5  also pick coke_can and place it to pos6\n\noutput :- Invalid(because we have designed our system in a static way you cannot move or do actions on same object more than once , i.e object coke_can is there more than once)",
        "input: move coke can from pos1 to pos2",
        "output: (and (located_at coke_can pos2) )",
        "input: move coke can to pos2",
        "output: (and (located_at coke_can pos2) )",
        "input: move bowl to pos4",
        "output: (and (located_at bowl pos4) )",
        "input: move beer to pos6",
        "output: (and (located_at beer pos6) )",
        "input: move pepsi_can from pos1 to pos2",
        "output: Invalid",
        "input: hello",
        "output: Invalid",
        "input: place coke_can to pos 6",
        "output: (and (located_at coke_can pos6) )",
        "input: can you pick table and place it in pos2",
        "output: invalid",
        "input: can you pick beer and place it in pos5",
        "output: (and (located_at beer pos5) )",
        "input: can you pick beer and place it in pos5 and also place coke_can to pos 6",
        "output: (and (located_at beer pos5) (located_at coke_can pos6) )",
        "input: can you pick beer and place it in pos5 and also place coke_can to pos 6 also move bowl to pos4",
        "output: (and (located_at beer pos5) (located_at coke_can pos6) (located_at bowl pos4) )",
        "input: can you pick can and place it in pos5",
        "output: (and (located_at coke_can pos5) )",
        "input: can you pick syrup and place it in pos5",
        "output: (and (located_at beer pos5) )",
        "input: Can you move Can to Position six.",
        "output: (and (located_at coke_can pos6) )",
        "input: place medicine to position nine",
        "output: (and (located_at beer pos9) )",
        "input: move medicine to position seven",
        "output: (and (located_at beer pos7) )",
        "input: "+voice_command,
        "output: ",
    ]


    response = model.generate_content(prompt_parts)    
    return response.text

class MinimalPublisher(Node):
    def __init__(self):
        super().__init__('minimal_publisher')
        self.publisher_ = self.create_publisher(String, 'topic', 10)
        self.trigger_detected = False
        self.transcription_start_time = 0

    def handle_transcript(self, msg):
        transcript = msg['metadata']['transcript'].lower()

        if TRIGGER_WORD in transcript and not self.trigger_detected:
            print(f"Trigger word '{TRIGGER_WORD}' detected!")
            self.trigger_detected = True
            self.transcription_start_time = time.time()

            # Clear the contents of the transcription file
            with open(TRANSCRIPTION_FILE, 'w') as file:
                file.write('')

        if self.trigger_detected:
            current_time = time.time()
            if current_time - self.transcription_start_time <= TRANSCRIPTION_DURATION:
                # Exclude the chunk containing the trigger word
                if TRIGGER_WORD not in transcript:
                    # Append the transcript to the file
                    with open(TRANSCRIPTION_FILE, 'a') as file:
                        file.write(msg['metadata']['transcript'] + '\n')
            else:
                print("Transcription stopped after 10 seconds.")
                self.trigger_detected = False
                response_text = pddl()
                # print("Transcription stopped after 10 seconds.")
                if "Invalid" not in response_text:
                	msg = String()
                	msg.data = response_text
                	self.publisher_.publish(msg)
                	self.get_logger().info('Publishing: "%s"' % msg.data)
                # print("Transcription stopped after 10 seconds.")
                else:
                    self.announce_invalid_command()
    def announce_invalid_command(self):
        engine = pyttsx3.init()
        engine.say("Invalid command")
        engine.runAndWait()
# DEVICE_INDEX = 3

def main(args=None):
    global DEVICE_INDEX
    DEVICE_INDEX = 3

    rclpy.init(args=args)
    minimal_publisher = MinimalPublisher()

    # Set up PyAudio
    p = pyaudio.PyAudio()
    if DEVICE_INDEX == 3:
        DEVICE_INDEX = p.get_default_input_device_info()['index']
        device_name = p.get_default_input_device_info()['name']
        DEF_SAMPLE_RATE = int(p.get_device_info_by_index(DEVICE_INDEX)['defaultSampleRate'])
        print(f"***\nIf you want to use a different microphone, update DEVICE_INDEX at the start of the code to one of the following:")
        # Filter out duplicates that are reported on some systems
        device_seen = set()
        for i in range(p.get_device_count()):
            if p.get_device_info_by_index(i)['name'] not in device_seen:
                device_seen.add(p.get_device_info_by_index(i)['name'])
                try:
                    supports_input = p.is_format_supported(DEF_SAMPLE_RATE, input_device=i, input_channels=1, input_format=pyaudio.paFloat32)
                except Exception:
                    supports_input = False
                if supports_input:
                    print(f"-- To use << {p.get_device_info_by_index(i)['name']} >>, set DEVICE_INDEX to {i}")
        print("***\n")

    SAMPLE_RATE = int(p.get_device_info_by_index(DEVICE_INDEX)['defaultSampleRate'])
    device_name = p.get_device_info_by_index(DEVICE_INDEX)['name']

    print(f"\nUsing << {device_name} >> which is DEVICE_INDEX {DEVICE_INDEX}")
    print("Starting transcription (type Ctrl-C to stop):")

    stream = p.open(format=pyaudio.paFloat32,
                    channels=1,
                    rate=SAMPLE_RATE,
                    input=True,
                    frames_per_buffer=CHUNK_SIZE,
                    input_device_index=DEVICE_INDEX,
                    stream_callback=stream_callback
    )

    # Define connection parameters
    conn = speechmatics.models.ConnectionSettings(
        url=CONNECTION_URL,
        auth_token=API_KEY,
    )

    # Create a transcription client
    ws = speechmatics.client.WebsocketClient(conn)

    # Define transcription parameters
    conf = speechmatics.models.TranscriptionConfig(
        language=LANGUAGE,
        enable_partials=True,
        max_delay=5,
    )

    # Check if transcription file exists, if not create one
    if not os.path.isfile(TRANSCRIPTION_FILE):
        open(TRANSCRIPTION_FILE, 'w').close()

    # Register the event handler for full transcript
    ws.add_event_handler(
        event_name=speechmatics.models.ServerMessageType.AddTranscript,
        event_handler=minimal_publisher.handle_transcript,
    )

    settings = speechmatics.models.AudioSettings()
    settings.encoding = "pcm_f32le"
    settings.sample_rate = SAMPLE_RATE
    settings.chunk_size = CHUNK_SIZE

    print(f"Waiting for trigger word '{TRIGGER_WORD}'...")
    try:
        ws.run_synchronously(audio_processor, conf, settings)
    except KeyboardInterrupt:
        print("\nTranscription stopped.")
    except HTTPStatusError as e:
        if e.response.status_code == 401:
            print('Invalid API key - Check your API_KEY at the top of the code!')
        else:
            raise e

    minimal_publisher.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()

