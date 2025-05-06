(function (window) {
    "use strict";

    // CSS for toast fade transitions
    const css = `
        .toast.fade { opacity: 0; transition: opacity 0.5s ease; }
        .toast.fade.show { opacity: 1; }
    `;
    const style = document.createElement("style");
    style.type = "text/css";
    if (style.styleSheet) {
        style.styleSheet.cssText = css;
    } else {
        style.appendChild(document.createTextNode(css));
    }
    document.head.appendChild(style);

    // Tab class definition
    class Tab {
        constructor(element) {
            this._element = element;
        }

        show() {
            const targetSelector = this._element.getAttribute("href");
            if (!targetSelector) return;

            const tabsContainer = this._element.closest(".nav");
            if (tabsContainer) {
                const activeTab = tabsContainer.querySelector(".nav-link.active");
                if (activeTab) {
                    activeTab.classList.remove("active");
                }
            }
            this._element.classList.add("active");

            const tabContent = document.querySelector(".tab-content");
            if (tabContent) {
                const activePane = tabContent.querySelector(".tab-pane.active");
                if (activePane) {
                    activePane.classList.remove("active", "show");
                }
                const targetPane = tabContent.querySelector(targetSelector);
                if (targetPane) {
                    targetPane.classList.add("active");
                    void targetPane.offsetWidth; // Force reflow for transition
                    targetPane.classList.add("show");
                }
            }
        }
    }

    // Toast class definition
    class Toast {
        constructor(element, options = {}) {
            this._element = element;
            this._delay = options.delay || 3000;
            if (!this._element.classList.contains("fade")) {
                this._element.classList.add("fade");
            }
            this._setupCloseButton();
        }

        _setupCloseButton() {
            const closeButton = this._element.querySelector(".btn-close");
            if (closeButton) {
                closeButton.addEventListener("click", (e) => {
                    e.preventDefault();
                    this.hide();
                });
            }
        }

        show() {
            this._element.classList.add("show");
            this._timeout = setTimeout(() => {
                this.hide();
            }, this._delay);
        }

        hide() {
            this._element.classList.remove("show");
            clearTimeout(this._timeout);
            setTimeout(() => {
                const event = new Event("hidden.bs.toast");
                this._element.dispatchEvent(event);
            }, 500);
        }
    }

    // Expose Tab and Toast classes to the window object
    window.bootstrap = {
        Tab,
        Toast,
    };

    // Event listener for tab clicks
    document.addEventListener("click", (e) => {
        let target = e.target;
        while (target && target !== document) {
            if (target.matches('[data-bs-toggle="pill"]')) {
                e.preventDefault();
                const tab = new window.bootstrap.Tab(target);
                tab.show();
                break;
            }
            target = target.parentElement;
        }
    });

    // Intersection Observer for toast elements
    const observer = new IntersectionObserver((entries) => {
        entries.forEach((entry) => {
            if (!entry.isIntersecting) {
                entry.target.classList.remove("show");
            }
        });
    });

    // Observe all toast elements
    document.querySelectorAll(".toast").forEach((toast) => {
        observer.observe(toast);
    });
})(window);
