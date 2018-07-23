(setq default-tab-width 2)

(add-hook 'python-mode-hook
          (function (lambda ()
                      (setq indent-tabs-mode nil
                            tab-width 2
														python-indent 2
														python-indent-offset 2))))
