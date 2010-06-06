// cs174, assignment 1 sample file (RIGHT HANDED)

camera {
  location  <0, 10, -14>
  up        <0,  1,  0>
  right     <1.33333, 0,  0>
  look_at   <0, 0, 0>
}


light_source {<15, 12, -20> color rgb <1.5, 1.5, 1.5>}


box { <-2, -2, -2>, <2, 2, 2>
  pigment { color rgb <1, 1, 0>}
  finish {ambient 0.2 diffuse 0.4}
}

sphere { <2, 1, -5>, 1
  pigment { color rgb <0.6, 0.0, 0.0>}
  finish {ambient 0.01 diffuse 0.7}
}


plane {<0, 1, 0>, -2
      pigment {color rgb <1, 1, 1>}
      finish {ambient 0.1 diffuse 0.8}
}

