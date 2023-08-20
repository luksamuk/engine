((nil 
  (eval . (let ((root (projectile-project-root)))
            (setq-local flycheck-clang-include-path
                        (list (concat root "include")))
	    (setq-local flycheck-gcc-include-path
                        (list (concat root "include")))))))
