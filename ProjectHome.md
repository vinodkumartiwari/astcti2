<table>
<tr>
<td><img src='http://astcti2.googlecode.com/svn/trunk/Graphics/asteriskcti_logo.png' /></td>
<td>
<h1>Introduction</h1>
<h2>AsteriskCTI</h2>
In my idea AsteriskCTI should be basically a multi-platform “easy to use” client/ server CTI solution integrated with the Open Source Digium’s Asterisk PBX Server.<br>
<br>
C.T.I. is an acronym of Computer Telephony Integration: so a CTI is a software that makes possible to transfer from the telephone interface to the client PC a set of useful  informations like calling party telephone number, digits grabbed by IVRs and so on. This information should be made available to a client software for database queries and/or other usage.<br>
</td>
</tr>
</table>
<table>
<tr>
<td>
Actually, the advent of Web 2.0 technologies has to reflect on the concept of CTI: today a lot of back-office softwares runs like web pages into browsers, and thus the most important innovation that a new CTI platform can carry is the web integration. Call Data generated on the PBX logic should be passed to a middle-ware able to perform the most disparate operations such as execute an external application or open a browser, allowing to transfer them the variables as telephone parameters.<br>
<br>
This new model make possible to build callcenter logic without the knowledge of complex API manageable only with advanced C/C++ skills, like TAPI.<br>
<br>
Finally, this application should be available and runnable, client and server, on all (or almost all) modern operating systems:  to achieve this objective I’ve selected, after trying various solutions (like mono/.net on GTK), to use the QT toolkit and – by consequence - C++ language. QT seems to be very easy to learn, very quick and stable,  really multiplatform and now also with a multiplatform IDE. The same sources can be ported from Win32 to Linux just by compiling and packaging them.<br>
<br>
<h2>Development</h2>
Project documentation <a href='http://astcti2.googlecode.com/svn/trunk/Docs/asterisk_cti.pdf'>is avaiable as PDF</a> and OpenOffice odt source in the SVN Area. <b>Take a moment to read it</b>

The project requires the following skills:<br>
<br>
<ul><li>C++ / Qt for AsteriskCTI Client and AsteriskCTI Server<br>
</li><li>PHP 5.x for AsteriskCTI Configurator web frontend</li></ul>

Actually the development effort is made on the <a href='http://code.google.com/p/astcti2/source/browse/#svn/trunk/AstCTIServer'>Server</a> part.<br>
<br>
<br>
<h2>Help Us</h2>
Like all opensource projects, also this needs developers to grow quick and stable.<br>
<br>
<a href='http://asteriskcti.brunosalzano.com/'>Some video tutorials and suggests</a> about development environment are avaiable. <b>Take a look and begin develop right now!</b>
</td>
</tr>
</table>


