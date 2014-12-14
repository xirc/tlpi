#!/usr/bin/env bash

(
    cp check_password_caps /tmp/check_password_caps
    cd /tmp

    sudo setcap 'cap_dac_read_search=p' check_password_caps
    getcap check_password_caps

    ./check_password_caps
)

exit 0
