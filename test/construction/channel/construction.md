# Timer

Implementation with priority queue is modified to use with lax. 

- slot
  - half of scheduled interval to be more accurate
  - next trigger time based sorting with a priority queue 
    - only checks timers to expire

The above implementation is good enough for games. 

When more efficient algorithm is required, look at following:

- https://www.snellman.net/blog/archive/2016-07-27-ratas-hierarchical-timer-wheel
  - a timing wheel. hierarchical.
  - timer callback

