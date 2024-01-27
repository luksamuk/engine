((nil 
  (eval . (let ((root (projectile-project-root)))
            (setq-local flycheck-clang-include-path
                        (list (concat root "src/include")
			      (concat root "include")
                              (concat root "imgui-1.89.9")
                              (concat root "imgui-1.89.9/backends")
                              (concat root "flecs-3.2.7")
                              (concat root "rapídxml-1.16")
                              (concat root "tomlplusplus-3.4.0")))
            (setq-local flycheck-gcc-include-path
                        (list (concat root "src/include")
			      (concat root "include")
                              (concat root "imgui-1.89.9")
                              (concat root "imgui-1.89.9/backends")
                              (concat root "flecs-3.2.7")
                              (concat root "rapídxml-1.16")
                              (concat root "tomlplusplus-3.4.0")))))))
