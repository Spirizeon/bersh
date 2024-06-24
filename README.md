![image](https://github.com/Spirizeon/bersh/assets/123345456/71540287-918f-4002-bff4-2d385e464eb9)

# bersh
A very simple POSIX shell written in C

## inbuilt features
- supports all basic GNU utils ✅
- piping ✅
- basic prompt user@host ✅
- tab-autocomplete ⚪
- input redirection ⚪

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
