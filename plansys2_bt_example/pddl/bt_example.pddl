(define (domain pnp)
(:requirements :typing :durative-actions) 

(:types         
    bot
    object
    position
    
)
(:predicates

    (at ?arm - bot ?pos - position)
    (located_at ?obj - object ?pos - position )
    (free ?pos - position )
    (path ?pos1 - position  ?pos2 - position )
    (holding ?arm - bot ?obj - object)
    (reachable ?arm - bot ?pos - position ) 
    (empty ?arm - bot)
    
)
  
(:durative-action pick_up
    :parameters
    (?arm - bot
     ?obj - object
     ?pos - position 
    )
    :duration (= ?duration 5)
    :condition
    (and
      (at start (at ?arm ?pos ))
      (at start (located_at ?obj ?pos ))
      (at start (empty ?arm))
      (over all (reachable ?arm ?pos ))
      )
    :effect
    (and
    (at end (not (located_at ?obj ?pos )))
    (at end (holding ?arm ?obj))
    (at end (not (empty ?arm))))
  )
 
   (:durative-action place
    :parameters
    (?arm - bot
     ?obj - object
     ?pos - position 
    )
    :duration ( = ?duration 5)
    :condition
    (and
      (at start (at ?arm ?pos ))
      (at start (holding ?arm ?obj))
      (over all (reachable ?arm ?pos ))
      (at start (free ?pos ))
     )
   
    :effect
    (and 
    (at end (not (holding ?arm ?obj)))
    (at end (located_at ?obj ?pos ))
    (at end (empty ?arm))
    (at end (not (free ?pos )))
    )
  )
    
  (:durative-action move
  :parameters
   (?arm - bot 
   ?pos1 - position 
   ?pos2 - position )
  :duration ( = ?duration 5) 
  :condition
     (and
        (at start (at ?arm ?pos1 ))
        (at start (path ?pos1 ?pos2 )) 
        (at start (reachable ?arm ?pos2 ))   
    )
  :effect 
     (and
        (at start (not (at ?arm ?pos1 )))
        (at end (at ?arm ?pos2 )))
     )
)     
