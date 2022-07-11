Gnomescroll
===========

Multiplayer voxel game 

Before compiling
================

Install waf  
Install python2  
Install sdl_image  
Install sdl2_image  
Install hiredis  
Install libdev  

Compiling
=========

Clone [Gnomescroll-Dependencies](https://github.com/Gnomescroll/Gnomescroll-Dependencies), 
and move or link it to `lib/` in the root of the Gnomescroll repo.

On Linux or Windows+Mingw:

```cd netclient```  
```python2 ./waf configure```  
```python2 ./waf```  

The file "run" is the client executable

```cd server```  
```python2 ./waf configure```  
```python2 ./waf```  

The file "run" is the server executable

MSVC and XCode project files are in `src/`


License
=======

Dual GPLv3 and proprietary license.



