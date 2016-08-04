# echoirlp
Basic steps to install EchoIRLP
Prerequisites:

    IRLP must already be installed and fully configured and working. EchoIRLP uses IRLP code.
    Your IRLP node must be running Debian versions of IRLP.
    Your node must have the compiler (GCC) and development software installed. This may not have been installed from the latest IRLP Linux software CDROM unless you chose to install everything.
    You must have registered your EchoLink node via Windows or the EchoLink web site. Your EchoLink node must be a -R (repeater) or -L (link) radio node. Once you have your EchoLink node working on the Windows client, you will use that configuration information in EchoIRLP install.

    It is recommended that you make a backup copy of IRLP using the script /home/irlp/scripts/backup_for_reinstall before installing EchoIRLP. Copy the backup file to a safe place. You can reinstall IRLP from the backup using an IRLP reinstall script.

EchoIRLP install modifies these IRLP files:
  /home/irlp/custom/rc.irlp 
  /home/irlp/custom/custom_decode 
  /home/irlp/custom/custom_on 
  /home/irlp/custom/custom_off 
  /home/irlp/custom/environment

Steps to install EchoIRLP software on your IRLP system:

    login as root
    wget -N --no-check-certificate https://raw.githubusercontent.com/wd5m/echoirlp/master/install/echo-install
    chmod +x ./echo-install
    ./echo-install
    Answer the prompts.
    Restart IRLP by running /home/irlp/custom/rc.irlp

Test by connecting to the EchoLink ECHOTEST service, EchoLink node 9999.
Known Issues

    Some have reported install failures, where the symptom is an empty tbd.conf file. So far, we have not been able to debug this. When this occurs, try installing again.

Anyone may browse and view the EchoIRLP code by opening their browser to https://github.com/wd5m/echoirlp/
