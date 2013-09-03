setInterval(function() {
    var auth_inputs = $('.Wt-auth-login-table input');
    if (auth_inputs.length == 3) {
		// login form
        var login_button = $('.Wt-auth-login-table button');
        if (login_button.data('save_login_and_password') != "1") {
            login_button.data('save_login_and_password', "1");
            var login_input = auth_inputs[0];
            var password_input = auth_inputs[1];
            var save_input = auth_inputs[2];
            try {
                if (window.localStorage.getItem('login').length > 0) {
                    login_input.value = window.localStorage.getItem('login');
                    password_input.value = window.localStorage.getItem('password');
                    save_input.checked = true;
                }
            } catch(e)
            { }
            login_button.click(function() {
				try {
                    if (save_input.checked && login_input.value != "" &&
							password_input.value != "") {
                        window.localStorage.setItem('login', login_input.value);
                        window.localStorage.setItem('password',
                            password_input.value);
                    } else {
                        window.localStorage.removeItem('login');
                        window.localStorage.removeItem('password');
                    }
				} catch(e)
				{ }
            });
        }
    } else if (auth_inputs.length == 0) {
		// logout button
        var logout_button = $('.Wt-auth-logged-in button');
        if (logout_button.data('save_login_and_password') != "1") {
            logout_button.data('save_login_and_password', "1");
            logout_button.click(function() {
				try {
					window.localStorage.removeItem('login');
					window.localStorage.removeItem('password');
				} catch(e)
				{ }
            });
		}
	}
}, 1000);

