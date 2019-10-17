# .bash_profile

# Get the aliases and functions
if [ -f ~/.bashrc ]; then
	. ~/.bashrc
fi

# User specific environment and startup programs

PATH=$PATH:$HOME/bin

export PATH

# Aliases for talk

alias ex1='./example-incorrect-label.sh 2>&1 | grep -Ev "^read$"'
alias ex2='./example-non-default-directory.sh 2>&1 | grep -Ev "^read$"'
alias ex3='./example-booleans.sh 2>&1 | grep -Ev "^read$"'
alias ex4='./example-non-default-port.sh 2>&1 | grep -Ev "^read$"'
