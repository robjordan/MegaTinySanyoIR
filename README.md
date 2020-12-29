# MegaTinySanyoIR
Using an ATTINY '1' series microcontroller to replace a remote control using the Sanyo LC7461 protocol.

We have a single remote for a Roberts radio. I fancied having a second, so I thought I'd use one of the common Arduino IR projects to create a gadget that could record and replay the relevant IR codes. It turns out that the common IR libraries can interpret codes from Sanyo LC7461 IR controllers but for some reason they can't send them. 

I created this little programme to use an ATTINY1614 to send the code for 'Sleep' to our Roberts radio. It's a very limited purpose, but it works. 

Some sources of inspiration:

- [Michael Higgins's ESP8266-HTTP-IR-Blaster](https://github.com/mdhiggins/ESP8266-HTTP-IR-Blaster). It's a great tool for recording IR codes, it correctly interpreted the Sanyo codes, which set me on the right track to generate them. I had unaccountable problems getting my ESP8266 NodeMCU to sit hapily on the WiFi upstairs in our house though, so I didn't persevere with it.
- [David Johnson-Davies's IR Wand](http://www.technoblogy.com/show?25TN), one of several great blog posts about IR protocols and microcontrollers. My hardware design is largely filched from David's wand. I added a 2N2222 transistor to allow the IR LED to be driven to a higher current, an idea that came from Michael Higgins.
- [Spence Konde's MegaTinyCore](https://github.com/SpenceKonde/megaTinyCore). Essential for writing Arduino IDE projects targetted on the newer series of ATTINYs, but also a valuable source of example code. This code is very largely based on his [PWM example](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/TakingOverTCA0.md), with a dash of [Power Save](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/PowerSave.md) and [Pin Interrupts](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/PinInterrupts.md) thrown in.

