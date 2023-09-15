((nil 
  (eval . (let ((root (projectile-project-root)))
            (setq-local flycheck-clang-include-path
                        (list (concat root "include")
			      (concat root "imgui-1.89.9")
			      (concat root "imgui-1.89.9/backends")))
	    (setq-local flycheck-gcc-include-path
			(list (concat root "include")
			      (concat root "imgui-1.89.9")
			      (concat root "imgui-1.89.9/backends")))))))
