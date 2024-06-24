# bersh
A very simple POSIX shell written in C

## inbuilt commands
- `cd` 
- `help` 
- `exit` 

## installation
> ⚠ this shell is still in development, setting it as your login shell may crash your system

```
git clone https://github.com/spirizeon/bersh 
cd bersh
chmod +x bersh 
mv bersh /bin/ 
chsh -s /bin/bersh 
grep "^${USER}" /etc/passwd
```
