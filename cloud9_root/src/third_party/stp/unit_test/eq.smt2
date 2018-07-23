(set-logic QF_BV)
(set-info :smt-lib-version 2.0)
(set-info :category "check")
(set-info :status sat)
(declare-fun v0 () (_ BitVec 2))
(declare-fun v1 () (_ BitVec 2))
(declare-fun v2 () (_ BitVec 2))
(declare-fun v3 () (_ BitVec 2))


(assert  
	(=	
		(bvor (concat v0 (_ bv0 2))  (concat (_ bv0 2) v1) )
		(bvxor (concat v2 (_ bv0 2))  (concat (_ bv0 2) v3) )
	)
)



(check-sat)
(exit)

