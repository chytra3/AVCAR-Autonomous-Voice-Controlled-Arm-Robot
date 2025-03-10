import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2

class ImageViewer(Node):
    def __init__(self):
        super().__init__('image_viewer')
        self.bridge = CvBridge()
        self.left_sub = self.create_subscription(Image, '/left/image_raw', self.left_callback, 10)
        self.right_sub = self.create_subscription(Image, '/right/image_raw', self.right_callback, 10)
        cv2.namedWindow("Left Image", cv2.WINDOW_NORMAL)
        cv2.namedWindow("Right Image", cv2.WINDOW_NORMAL)

    def left_callback(self, msg):
        cv_image = self.bridge.imgmsg_to_cv2(msg, "bgr8")
        cv2.imshow("Left Image", cv_image)
        cv2.waitKey(1)

    def right_callback(self, msg):
        cv_image = self.bridge.imgmsg_to_cv2(msg, "bgr8")
        cv2.imshow("Right Image", cv_image)
        cv2.waitKey(1)

def main(args=None):
    rclpy.init(args=args)
    image_viewer = ImageViewer()
    rclpy.spin(image_viewer)
    image_viewer.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()

