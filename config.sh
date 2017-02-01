
#!/usr/bin/env bash

# Set colors
GREEN='\033[1;32m'
CYAN='\033[1;36m'
NC='\033[0m'

printf "${GREEN}----${NC}"

echo

## Set user email
#read -p "Set your email adress: " email
#git config --local user.email $email
#printf "${CYAN}User email was set.${NC}"

#echo
#echo

## Set user name
#read -p "Set your user name: " name
#git config --local user.name $name
#printf "${CYAN}User name was set.${NC}"

#echo
#echo

# Connect .gitconfig
git config --local include.path ../.gitconfig
rm .git/index
git reset --hard HEAD
git pull
printf "${GREEN}Git config was successfully set.${NC}"
echo
printf "${GREEN}----${NC}"
echo