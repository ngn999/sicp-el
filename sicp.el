;; This buffer is for notes you don't want to save, and for Lisp evaluation.
;; If you want to create a file, visit that file with C-x C-f,
;; then enter the text in that file's own buffer.

;;;; accumulate
(defun accumulate (op init seq)
  (if (null seq)
      init
    (funcall op (car seq)
        (accumulate op init (cdr seq))))
)

;; (accumulate '+ 0 '(1 2 3 4))

(defun filter (p seq)
  (if (null seq)
      nil
    (if (funcall p (car seq))
        (cons (car seq)
              (filter p (cdr seq)))
      (filter p (cdr seq)))))
(filter 'oddp '(1 2 3 4 ))

; (mapcar '1+ '(1 2 3))
; (mapcar 'sqrt '(1 2 3 4 5 6 7))

(defun flatmap (proc seq)
  (accumulate 'append nil (mapcar proc seq)))

(defun enumerate-interval (begin end)
  (if (> begin end)
      nil
    (cons begin 
          (enumerate-interval (1+ begin) end))))
;;; (enumerate-interval 1 100) ;;; OK


;;;;; flatmap -- OK
;; (flatmap 
;;  (lambda (i)
;;     (mapcar (lambda (j)
;;                (list j i))
;;             (enumerate-interval 1 (1- i)))
;;     )
;;  (enumerate-interval 1 6)
;;  ) 


;;; (remove 2 '(1 2 3)) ;;; -- OK

;;;
(defun perm (seq)
  (if (null seq)
      (list nil)
    (flatmap (lambda (x)
               (mapcar (lambda (p) (cons x p))
                       (perm (remove x seq)))) 
             seq)))
;;; (perm '(1 2 3 4))  ;; ok

(defun fuck (seq)
  (if (= (length seq) 1)
      (car seq)
    (progn 
      (if (= (fuck (cons (+ (car seq))
                         (cdr seq))))))
    )
  )

(fuck '(1 2 3 4))
;;(= 2 3)
;;(/ 3.0 2)
;;; (eval '(funcall '1+ 2))

(cond )
(null nil)
(length '(2))
(car '(2))