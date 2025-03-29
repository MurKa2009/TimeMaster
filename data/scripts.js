(function (window) {
    "use strict";

    var css = ".toast.fade { opacity: 0; transition: opacity 0.5s ease; } " + ".toast.fade.show { opacity: 1; } ";
    var style = document.createElement("style");
    style.type = "text/css";
    if (style.styleSheet) {
        style.styleSheet.cssText = css;
    } else {
        style.appendChild(document.createTextNode(css));
    }
    document.head.appendChild(style);

    function Tab(element) {
        this._element = element;
    }

    Tab.prototype.show = function () {
        var targetSelector = this._element.getAttribute("href");
        if (!targetSelector) return;

        var tabsContainer = this._element.closest(".nav");
        if (tabsContainer) {
            var activeTab = tabsContainer.querySelector(".nav-link.active");
            if (activeTab) {
                activeTab.classList.remove("active");
            }
        }
        this._element.classList.add("active");

        var tabContent = document.querySelector(".tab-content");
        if (tabContent) {
            var activePane = tabContent.querySelector(".tab-pane.active");
            if (activePane) {
                activePane.classList.remove("active", "show");
            }
            var targetPane = tabContent.querySelector(targetSelector);
            if (targetPane) {
                targetPane.classList.add("active");
                void targetPane.offsetWidth;
                targetPane.classList.add("show");
            }
        }
    };

    function Toast(element, options) {
        this._element = element;
        this._delay = options && options.delay ? options.delay : 3000;
        if (!this._element.classList.contains("fade")) {
            this._element.classList.add("fade");
        }
        var self = this;
        var closeButton = this._element.querySelector(".btn-close");
        if (closeButton) {
            closeButton.addEventListener("click", function (e) {
                e.preventDefault();
                self.hide();
            });
        }
    }

    Toast.prototype.show = function () {
        var self = this;
        this._element.classList.add("show");
        this._timeout = setTimeout(function () {
            self.hide();
        }, this._delay);
    };

    Toast.prototype.hide = function () {
        var self = this;
        this._element.classList.remove("show");
        clearTimeout(this._timeout);
        setTimeout(function () {
            var event;
            if (typeof Event === "function") {
                event = new Event("hidden.bs.toast");
            } else {
                event = document.createEvent("Event");
                event.initEvent("hidden.bs.toast", true, true);
            }
            self._element.dispatchEvent(event);
        }, 500);
    };

    window.bootstrap = {
        Tab: Tab,
        Toast: Toast,
    };

    document.addEventListener("click", function (e) {
        var target = e.target;
        while (target && target !== document) {
            if (target.matches('[data-bs-toggle="pill"]')) {
                e.preventDefault();
                var tab = new bootstrap.Tab(target);
                tab.show();
                break;
            }
            target = target.parentElement;
        }
    });

    // Intersection Observer для toast
    var observer = new IntersectionObserver(function (entries) {
        entries.forEach(function (entry) {
            if (!entry.isIntersecting) {
                entry.target.classList.remove("show");
            }
        });
    });

    document.querySelectorAll(".toast").forEach(function (toast) {
        observer.observe(toast);
    });
})(window);
