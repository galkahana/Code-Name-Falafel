# Code-Name-Falafel
Allow running C++ library full tree of classes from DotNet. C++ Library to automatically build .net bridge classes


So before the discovery of NodeJS i was looking for a method to "easily" produce a C# driver to the C++ PDFWriter (hummus) library. 
Point was to provide Hummus to more than just C++ programmers but also to my pals using C#. Why C#? Got most requests for C# then plus I myself was playing with C# quite a bit at the time.

This module goal was to do the driver building automatically. Parse the C++ headers to constructs and then iterate them and create bridge files based on those constructs.
The same method could work for any language that allows building "bridges" over C++ (native) like Java...and well NodeJS (and Python and whatever). From another point of view it could work for any library.

I got to have a fairly complete parser for H files (only to be told later that there were such parser already available for me hahah. but still this one builds constructs im combortable with so...didn't bother me much). 
oh boy that was fun reading the specs of C++ and implementing them. You can see the tests playground for some examples of the successful attempts to parse h files.

Problem was that around that time I was overcome by a slowly increasing existantial conflict which made me drop the project. Later I found out about NodeJS and figured I can easily write (manually...) A wrapper for Hummus 
for NodeJS...and then anyone who wants to work with Hummus in any language can use NodeJS as a service and make calls to it via net or something, as it's fairly easy to setup such a service. And so this project was left abandoned in favor of [HummusJS](https://github.com/galkahana/HummusJS).
