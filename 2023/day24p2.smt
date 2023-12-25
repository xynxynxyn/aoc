(declare-const X Int)
(declare-const Y Int)
(declare-const Z Int)
(declare-const dx Int)
(declare-const dy Int)
(declare-const dz Int)
(declare-const t0 Int)
(declare-const t1 Int)
(declare-const t2 Int)

(assert (= (+ 19 (* -2 t0)) (+ (* dx t0) X)))
(assert (= (+ 13 (*  1 t0)) (+ (* dy t0) Y)))
(assert (= (+ 30 (* -2 t0)) (+ (* dz t0) Z)))

(assert (= (+ 18 (* -1 t1)) (+ (* dx t1) X)))
(assert (= (+ 19 (* -1 t1)) (+ (* dy t1) Y)))
(assert (= (+ 22 (* -2 t1)) (+ (* dz t1) Z)))

(assert (= (+ 20 (* -2 t2)) (+ (* dx t2) X)))
(assert (= (+ 25 (* -2 t2)) (+ (* dy t2) Y)))
(assert (= (+ 34 (* -4 t2)) (+ (* dz t2) Z)))

(check-sat)
(get-model)
