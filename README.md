# tick-led
An LED clock made of daisy-chained RGB LEDs

![tick-led](https://github.com/ole00/tick-led/raw/master/img/tl000-tick-led.jpg "tick-led")

The picture above lacks cotrast and does not show the vibrant colors of the LED display.

**Tick-led features:**
* shows time on a colorfull display
* support for different color schemes and random color schemes
* configurable clock hand animations (3 options)
* supports time zones and daylight saving time
* time synchronisation over WiFi (NTP protocol)
* up to 5 alarms
* buzzer with simple RTTL melodies (for alarms)
* accelerometer to check clock motion (stops the alarm sound)
* brightness options
* dimming options to reduce glare at night time
* requires 5v USB-C power supply (phone charger)
* service button to enter setup
* options are configuarable over WiFi (web page) or serial port (command prompt)
* supports firmware upgrades over serial port (via service button press)
* fully open source and open hardware, it is a DYI project
* based on ESP32 S2 Mini MCU board - widely available.
* fully open source and open hardware. You can find the firmware souce code, pcb design
  files (made by open source CAD software Geda PCB), case desgin files (made for open source
  3D CAD desgin - OpenScad) and pdf schematic in this git repo.


**Build complexity:**
* Medium to low
   - Medium, if you want to have accelerometer and buzzer. Requires SMD soldering skills.
   - Low, if the accelerometer and buzzer is is not populated. The clock will function
     without these parts, but the alarm feature will not work in such case. Requires basic
     through-hole soldering skills.
* Requires soldering - the MCU board needs to be soldered to the base board.
  At least some basic soldering skills are required to solder the MCU pins (0.1" /2.54mm pitch)
* To solder accelerometer and buzer you'll need to have intermediate
  soldering skills and be comfortable to solder SMT 0603 and QFN components.
* The LED board contains 133 individual RGB LEDs, but the whole LED board
  can be produced and assembled (parts soldered to the board) by an SMT assebly service.
  All required files are provided for JLCPCB PCB Assembly service. Just upload them to 
  the service, pay for it and wait for delivery to get the board as shown on the
  top right image. You will need to solder the grove connector (through hole, 2mm pitch)
  which requires basic soldering skills.
* The case requies 3D printed parts. These can be produced on a home 3D printer or can be sent
  to a 3d printing service. The top image shows 3d printed case produced by JLCPCB 3D printing
  service - material MJF nylon, natural gray.
* The rest of the assembly then requires basic mechanical skills and equipment for glueing, attaching bolts
  and screwing.
* Software wise, currently it requires a PC with Arduino IDE to compile and program the firmware to the MCU board. In the future
  I'll prepare a simpler method of uploading the precompiled firmware binary to the MCU board with
  less hassle.


**Building the clock:**

 The clock building steps can be split to 6 groups of activities:
 1) planning the build, based on your skills and options
 2) ordering parts
 3) soldering of electronic parts
 4) 3d printing - optional if the case is printed by an online 3d printing service
 5) clock assembly
 6) complation and upload of the firmware to the MCU board

 
**1) Planning your build**
------------------------------
There is several combinations of options how how you can handle the whole build. If you are good at soldering SMT
you can solder the whole electronics yourself. If you have just basic soldering skills you can chose to use PCB assebly
online service for the LED board and omit the accelerometer and buzzer on the MCU host board. You'll also have to decide
whether you can 3d print the case locally or whether wou'll use a 3d printing service. Producing the PCB boards and 3d printing
at home may generally take longer time, but is usually cheaper (if you have experience) and probably more rewarding in the end.
Using the 3d printing and PCB assebly services is more expensive, but give you better and more predictable results (time wise,
print quality and finish wise). 
During the development I did all the above options (SMT soldering at home, 3d printing and then using online services),
but I'd recommend to most of the people (unless you like a challenge) to bite the bullet and pay
the PCB assembly for the LED board. The reason for that is the LEDs are quite small (2mm x 2mm), they are fragile heat wise
(excess heat during soldering can damage them) and require precise placement (you'll need to pick & place 133 LEDS
and 36 SMT capacitors).

Here is my recommedned plan:
* Order the LED board assembly at the online PCB assembly service.
* Order the MCU host board, clock face board and PCB brackets board in the same PCB manufacturing house where you order the LED board
  to save on the shipping costs.
* Decide whether you will use accelerometer (2 accelerometer options exist) and buzzer - based on your skills. If you do
  want them, then definitelly order a SMT stencil with the MCU host board (while ordering the MCU host board atc.)
* Decide whether you want to 3d print the case yourself or whether you''ll use an online service
* Check the current prices and your budget. The cheapest option (all parts produced and soldered at home) will probably cost you
about $60. The expensive option (LED board PCB assembly and 3d printing via online services) will be about $110 in total. The
costs are estimated in 2022/11 prices and may be different based on the country where you live and the current prices of the 
components and manufacturing services.
 
 
**2) Ordering parts**
----------------------------------
Part ordering reflects you plan from the step 1). I'll describe the recommended plan by using the SMT assembly service
for the LED board and fully populated MCU Host board (not produced via SMT assembly).

**Starting with electronic you'll need to:**
* order the LED pcb and its SMT assembly. Required quantity: 1. Files used for ordering: <br>
  gerbers/clk_led_fab_r03.zip : 2 layer board, 1.6 mm thickness, any finish and color <br> 
  gerbers/clk_led_v3_bom.xlsx : bill of materials for JLCPCB SMT assembly <br>
  gerbers/clk_led_v3_placement.xlsx : component placement file for JLCPCB SMT assmebly <br>
  
  If using JLCPCB SMT assembly for the first time make sure you select their $9 off Coupon during
  checkout.
  
* order the MCU Host/mounting board. Required quantity 1. Files used for ordering: <br>
  gerbers/clk_mcu_fab_r03.zip : 2 layer board, 1.6 mm thickness, any finish or color. <br>
  Order the SMT Stencil with this board. Ensure you selected a custom size of the 
  stencil. If the stencil is too large the fab house will send you 2 parcels which will
  cost you more. A good size for this stencil is 120 mm x 140 mm. The stencil is
  optional if you do not need the accelerometer and buzzer. You will still need the 
  MCU Host/mounting PCB though!

* order the clock face PCB. Required quantity 1. Files used for ordering: <br>
  gerbers/clk_face_fab_r01.zip : 2 layer board, 1.6 mm thickness, dark color (black or purple) <br>
  ! Ensure to select an option to not to print an PCB order number on this board, or else
  you'll see it on the clock face. It's possible to scratch/sand it off later on, but save 
  yourself a hassle.
  
* order the clock PCB brackets. Required quantity 2. Files used for ordering: <br>
  gerbers/clk_bracket_fab_r02.zip <br>
  This board is optional if you have a 3D printer and you plan to be printing yourself.
  In such case the brackets can be 3d printed and the saving is around $8. You could
  also save by using 3d printing service, but the savings will be smaller (approx $3-4).
  
* order the MCU development board Wemos S2 Mini. <br>
  The info page with the Online Shop can be found here: https://www.wemos.cc/en/latest/s2/s2_mini.html <br>
  Click on the Online Shop (on the left panel) and select the
  S2 Mini V1.0.0 - LOLIN WIFI IOT board from the list.
  
  **! A big warning !** This board seems to be popular so several clones emerged on ebay
  and aliexpress with varied build quality. Some clones can't establish WiFi connection!
  If you must buy this board from an alternative source check the S2 Mini clone guide
  that might help you to spot a clone.
 
  
* order the componets for the MCU Host board.  <br>
  Note that if you don't need accelerometer and the buzzer then you won't need these 
  compoenets.

  * 3-axis accelerometer: has 2 options, only one accelerometer option is needed. <br>
    Quantity 1: <br>
    Order either: <br>
    MC3479 <br>
      https://www.mouser.co.uk/ProductDetail/MEMSIC/MC3479?qs=OlC7AqGiEDmYmlaMvA9fTA%3D%3D <br>
      https://www.digikey.co.uk/en/products/detail/MC3479/3502-MC3479CT-ND/15292809 <br>
      U2 <br>
    MMA8491Q <br>
      https://uk.farnell.com/nxp/mma8491qr1/accelerometer-3-axis-8g-12qfn/dp/2291592 <br>
      https://www.mouser.co.uk/ProductDetail/NXP-Semiconductors/MMA8491QR1 <br>
      U1 <br>
      This one is no longer manufactured, but I had a bunch of them in my drawer
      so I used it. It has a larger footprint so it is a bit easier to solder.

   * ceramic capacitor 0.1uF 6V (or more), SMT, 0805, Quantity: 3 <br>
     C3, C4, C6 (or C2) <br>
     Note: if you have 0603 size already in your drawer, that should fit as well.
   * ceramic capacitor 4.7uF 6V (or more) 20%, 0805, Quantity: 1
     C37 on the LED board
     Note: any similar cap between 1uF to 10uF should do the job as well.     
   * optional: ceramic capacitor 1uF 6V (or more), SMT, 0603, Quantity 1 <br>
     Only if you use MMA8491Q accelerometer (cap C1 on MCU host board).
   * resistor 4k7, up to 5%, SMT, 0603, Quantity 2. <br>
     R1, R2
   * resistor 220R, up to 5%, SMT, 0603, Quantity 2 <br>
     R4, R6
   * resistor 1k, up to 5%, SMT, 0603, Quantity 1 <br>
     R3
   * SSTA06HZGT116 : BJT NPN Transistor (or similar), SOT-23, Quantity 1 <br>
     Q1
   * PS1240P02BT : Piezo buzzer 4KHZ 12.2 mm (or similar), Quantity 1
   

**Next are 3D printed parts for the clock case.**

   I prepared 2 sets of STL files which will be used depending on the 
   3D printing service. In case of printing at home you'll use
   an STL file with 100p appendix. That means the 3d model is of its 100% size.
   There is also the same 3D model exported with appendix 996p. Such model
   size is 99.6% - these are specifically for JLCPCB 3D printing service.
   The reason for making them a bit smaller is that the JLCPBB service
   prints the 3D models a tiny bit bigger (possibly to compensate for the sanding losses)
   but that makes the bracket holes and the hole for plexi glass too big which
   causes issues during clock assembly. So, use 996p files for JLCPCB
   3D printing service only (I tried SLA and Nylon options - both print-outs
   were bigger). When your 3d printing service offers a "Sanding" option then
   use it for the front and back case, as the finish of the parts is much smoother.
   
   * front case: case_3d_print/clock_case_front_v6_100p.stl
   * back case : case_3d_print/clock_case_back_v6_100p.stl
   * clock button : case_3d_print/clock_button.stl
   * clock button clip : case_3d_print/clock_butclip.stl
   * optionally 2 brackets: case_3d_print/clock_pcb_bracket.stl
    These are used instead of PCB brackets.
 
 
**Then order the plexi glass and bolts and nuts.**

All ebay links are for reference only (although these are the ones I used).
Use your local online shops for sourcing these parts.

   * dark gray tinted (Grey 923) laser cut perspex/plexi glass, 100 mm diameter, 3mm thickness. <br>
    https://www.ebay.co.uk/itm/254420100726?var=554040638251 <br>
    Ensure this is tinted (not opaque) plexi glass so that the LEDs can shine through it.
    
   * Grove - Universal 4 pin connectors, shrouded pin headers. Quantity 1 <br>
   https://www.ebay.co.uk/itm/142915378526
   
   * Seeed 110990027 Grove - Universal 4 Pin Grove cable with connectors. Quantity 1 <br>
   https://www.ebay.co.uk/itm/125432690638
   
   * M3 Hex bolt, 16mm. Quantity 4 <br>
   It needs to have Hex head and exactly 16 mm <br>
   https://www.ebay.co.uk/itm/264695481809?var=564502857692
   
   * M3 nuts. Quantity 16 <br>
   https://www.ebay.co.uk/itm/124341716846?var=425227838005
   
   * M3 hexagon dome nuts. Quantity 4 <br>
   https://www.ebay.co.uk/itm/114307049435?var=414558276018
   
   * M3 flat washers. Quantity 16 <br>
   https://www.ebay.co.uk/itm/150802639052?var=450075490640
   
   * M3 Solid brass slotted head screws. Length 12mm. Quantity 4 <br>
   https://www.ebay.co.uk/itm/161469038663?var=460504478738 <br>
   Must be brass - to be able to solder it to the clock PCB.
   10 mm or 16 mm length should work as well.
   You could possibly combine these and M3 Hex bolts above with this
   M3 hex brass bolts instead (quantity 8, length 16mm) <br>
   https://www.ebay.co.uk/itm/363522568395?var=632744925363

   * M2.6 Self tapping screw, 5 mm length. Quantity 2 <br>
   https://www.ebay.co.uk/itm/324102426977?hash=item4b76029161


**3) soldering of electronics**
---------------------------------

The LED board is (presumably) soldered for you by the SMT assembly service, but the
bottom side needs some additional soldering:
* Solder a 4.7uF (or similar) SMT ceramic capacitor on the bottom side, C37
* Solder the through hole Grove shrouded pin header.
  Ensure the pins and shrouding plastic is on the bottom side (without LEDs),
  therefore the solder joints are on the top side (with LEDs).
  The notch on the shrounding must be positioned up. <br>
  See img/tl002-led_pcb_installed.jpg for reference.
  
The MCU Host board requires soldering of SMT components (as a first stage) and
also the MCU board itself (the second stage)

* For SMT soldering use the SMT stencil and apply the solder paste. Pick & place
 the SMT component on the PCB then bake the board on a heat platform or use
 a hot air gun for soldering. Note that soldering of SMT parts is optional. If you 
 do not solder them the Accelerometer and the Buzzer functionality is lost, but the
 clock should work normally.

* For Through Hole parts: solder the buzzer and then solder the MCU board
  on top of the pin headers. The use of pin headers is required in order to
  fit the USB-C connector nicely into the USB-C hole in the clock casing.
  The way I soldered the board was: <br>
    * first put the header pins to the MCU Host board (do not solder them yet),
     and place the MCU board on top of it. Make sure the MCU board sits nicely and
     evenly on the pins and that the USB-C is positioned on the narrower part of the 
     host board (as indicated on the PCB silk screen). 
   * Then solder the pins on top of the (purple) MCU board.
   * Once that's done, secure the MCU board on the Host board with a blu-tack or 
     with a tape and turn the Host board upside down. Solder just one pin of the MCU board 
     and check whether the MCU board is positioned tightly and evenly on the board.
     If not, heat-up the soldered pin and press on the MCU board to squeeze it tight
     with the Host board. Once you are happy with the MCU board position finish
     soldering of the rest of the pins.
* Use multimeter to do continuity check between VCC and Ground (5V Rail).
  Use the holes on the left side of the Host board marked VCC and GND, ensure they
  are not shorted (no continuity check beep is audible)
* Use multimeter to do continuity check between 3.3V and Ground.
  Use the solder pads either on the C1 or C6, ensure they
  are not shorted. No continuity check beep must be is audible. If it is, it may be just a very short
  blip - that's OK because of the installed capacitor.

* Cut the grove cable in half evenly (cable length for installation will be ~ 95mm (~ 4").
  Remove the yellow cable from the connector - use a needle or tip of sharp tweezers
  to lift the clip on the connector and then pull out the Yellow cable. Remove about 4 mm of the
  isolation from the wires, tin the exposed metal strands and solder the 3
  wires to the holes on the left side of the Host PCB. 
   * Pin marked VCC - solder the red wire
   * Pin marked GND - solder the black wire
   * Pin marked DAT - solder the white wire

* Solder the 4 brass bolts to the bottom side of the Clock Face PCB. You many need to clean (with IPA) or sand the
  heads of the bolts to make sure the solder sticks well with the brass. Apply solder paste to the solder pads,
  center the brass bolts on the footprints as best you can and use a hot plate or hot air gun to solder the bolts
  in place. Themperature around 220C should be sufficient to solder the bolts when using SMT solder paste.
  Alternatively, use a chunky soldering iron with high temperature (400C) to solder the bolts. <br>
  *Caution:* the bolts will accumulate a lot of heat so let the whole board to cool down sufficiently
  before touching the bolts or the pcb after soldering. If you need to adjust the posititon of the bolts during
  soldering never touch the bolts directly - always use metal tweezers to prevent a skin burn! <br>
  See img/tl007-face_fixing_bolts.jpg for reference.

* Optional but recommended: install a patch wire between GPIO14 and Reset button (pin closer to the GPIO14)
  on the MCU board. This patch will allow you to upgrade the firmware while the case is closed.
  If you do not apply the patch, then the FW upgrades will still be possible, but you'll need
  to open the case and press both the GPIO0 Button and the Reset button before the upgrade.
  What this patch does is - it allows the firmware to reset the board via GPIO0. If such reset
  is done by holding the GPIO0 button then the MCU board will reboot itself into a bootloader.
 
* Check the img/tl004-mcu_pcb_installed_detail.jpg image for reference.
 
  
  
**4) 3D Printing:**
------------------
If you've used a 3D printing service as described in the Ordering section, you may skip this part.
* printing material: so far the best material (in my opinion) is the MJF nylon option, it gives a bit coarse
  finish but can withstand higher temperatures (up to 100C). The disadvantage seems it attracts finger prints
  which make darker spots on the surface. The SLS Nylon as printed by JLCPCB is light and can also withstand
  higher temepratures (up to 147C), but the finish is unplesant to touch so I would not
  recommend it. The second best option (in my opininon) on the JLCPCB print page is SLA LEDO 6060 Black Resin.
  After sanding it has really nice smooth finish (better than MJF nylon) and does not seem to attract fingerprints.
  The disadvantage is relatively low temperature durability (up to 58C), so the clock should not be placed
  on a windowsill to prevent case deformations or melting caused by the long exposure to sun rays.
* Color: in my opinion the black or dark gray seems to be the best option. At least the front case part should be opaque
  not to leak the LED light around the PCB.
* Import the stl files (see Ordering section) into your favorite 3D slicer, add supports if required,
  do the slicicng and print the parts.
* I recommend to start with the front case. After printing the front case check its Width dimension which should be 
  exactly 12mm. If you measure a value close to 120 mmm (+- 0.5mm), try to put the plexiglass in the hole whether it fits.
  If it fits, but not tightly decide whether to scale the model a bit down and reprint. If it does not
  fit you have 2 options, either to sand the plexi glass down to a smaller diameter or scale up the
  3D model and reprint. 
* Print the back case. If you scaled the front case up or down, use the same scaling ratio for the
  back case as well. That is important so that the front and back parts mate nicely without gaps, and also to ensure
  the mounting holes will match the distance of bolts which will are attached on the front case.
* Optionaly sand the front and back case. If you are using 3d printing service and it provides sanding option, then 
  use it and let them sand it for you.
* print the button and the button clips
* optionally print 2 pcb brackets for fixing the LED board with the front case.


**5) clock assembly**
----------------------

* glueing the plexi glass
  * !! wear an eye protection and protective gloves for this step. Super Glue can cause serious health damage
    if it gets into your eyes or on your skin !!
  * !! Make sure the room is well ventialed, avoid breathing the vapors from Super Glue !!
  * remove the protective foil from both sides of the plexiglass
  * place the Front case on a desk facing the engraved text up
  * do the final check that the plexi glass fits the hole in the Front case well without applying force.
    Remove the plexiglass from the hole after the check.
  * put a small droplets of  Super Glue on a tothpick. Carefully transfer the Super Glue droplet
    on the recessed rim of the plexi glass hole. Use the Super Glue in such way on 5 - 6 spots
    evenly spaced around the rim of the plexiglass hole.
  * fit the plexi glass into the hole and keep it pressed lightly for ~ 1 minute, then release the pressure.
  * leave the Super Glue to dry for 60 minutes - that is important to prevent leaks from the squeezed out
    SG doplets to accidentally stain the glass or the case.
  * !! Do not turn the Front Case with the freshly installed plexi glass upside down or to a side.
    If you do that, the wet glue will ooze out and stain the plexiglass !! Trust me, I've ruined
    a case and the plexi glass by just doing that. Let the SG to dry for at least an hour. After that
    it can be used for further assmebly.

* assemble the LED board:
  * join the Face PCB with the LED PCB by sliding the brass bolts into the holes in the LED pcb.
    The LEDs on the LED PCB should be covered by the Face PCB. If the bolts are not well positioned
    on the Face PCB or are not quite prependicular to the PCB, there will be a dificulty joing the 2 PCB. In such case 
    decide whether to fix the soldering of the bolts or use a round file to make the mounting holes a bit bigger.
    Ensure the LED board sits directly on top of the bolt's heads and there is no additional gap between the boards.
    The total gap between the 2 PCBs should be 2mm. Use 4 x M3 nuts to secure the Face and the LED PCBs.
    
  * add PCB brackets to the LED board. Use washers and nuts to secure the brackets. Leave the nuts lose (for now) to make
    the further assembly easier.
    
* attach bolts to the Front case.
   * Use the M3 Hex head bolts (16mm length) and put the bolt head through the eyelet hole.
   * Rotate the bolt in the eylet so that its head walls are flat on the left and right sides and 
     sharp angled edges of the bolt head are on the top and bottom. In other words: the bolt head sides
     must be parallel with the left and right side of the case.
   * pull the bolt into the eylet slot. Note that the bolt head must be correctly rotated (see above) in order 
     to squeze the bolt through to the very end of the slot.
   * secure the bolts with washer and nuts
   * see img/tl014-front_case_bolts.jpg for reference

* attach the LED board with brackets to the Front case
   * clean any smudges or dust from the internal side of the plexiglass
   * slide the outer bracket holes with the LED board through the bolts on the front case.
     Leave the bracket nuts loose for easier fit.
   * Once the LED obard is in place, secure the bolts with nuts.
   * Tighten all the nuts.
   * see img/tl002-led_pcb_installed.jpg for reference

* attach the MCU Host board
  * slide in the MCU Host board into the slot on the bottom of the Back case. You may need to trim the
    solder joints on the buzzer legs to fit it easily.
  * secure the MCU Host board with M2.6 x 5mm self tapping screws
  * insert the 3D printed button piece into the case hole and into the button slot.
  * glue the button clip on top of the button slot
  * see img/tl003-mcu_pcb_installed.jpg for reference
  
* case closure
  * plug in the Grove cable into the LED board
  * close the Front and Back case together, ensure the engraving is on the bottom side
  * put washers on the bolts and secure the case with M3 hexagon dome nuts. Do not overtight.
  
**6) complation and upload of the firmware to the MCU board**
--------------------------------------------------------------

  * install and run Arduino IDE 1.8.X
  * enter Menu->Tools->Board...->Board Manager
  * install board files for: esp32 by Espressif Systems (mine is version 2.0.2)
  * connect your PC and tick-led with a USB-C cable
  * open the arduino_fw/tick-led.ino file
  * enter Menu0>Tools->Board... and select ESP32S2 Dev Module
  * select the rest of the board options as depicted in the img/tl015-arduino_ide_board.jpg
  * ensure the Port: shows your connected tick-led serial port
  * enter the MCU bootloader
    * if uploading the FW for the first time or if the boot patch wire is not installed
      you will have to:
      * open the case,
      * diconnect the internal cable from the LED board
      * press and hold both MCU buttons pressed
      * release the Reset button while still pressing the 0 button 
      * release the 0 button after a second
    * if updating the FW from a previous tick-led FW AND the patch wire is installed
      * press and hold the service button for ~ 4 seconds to enter setup (Blue wedge is displayed)
      * press and hold the service button for another ~ 10 seconds to enter FW upgrade mode 
        (a small yellow ring is displayed)
    
  * run Menu->Sketch->Upload (CTRL+U) to upload firmware to the tick-led MCU. Ensure all serial
     terminals were closed before you start the upload. The upload is done when the following text
     (or similar is displayed):
    <pre>
    ...
    Writing at 0x000bdc1e... (99 %)
    Writing at 0x000be6f6... (100 %)
    Wrote 716896 bytes (458207 compressed) at 0x00010000 in 8.1 seconds (effective 711.8 kbit/s)...
    Hash of data verified.
    Compressed 3072 bytes to 128...
    Writing at 0x00008000... (100 %)
    Wrote 3072 bytes (128 compressed) at 0x00008000 in 0.1 seconds (effective 397.2 kbit/s)...
    Hash of data verified.

    Leaving...
    ERROR: ESP32-S2FH32 chip was placed into download mode using GPIO0.
    esptool.py can not exit the download mode over USB. To run the app, reset the chip manually.
    To suppress this error, set --after option to 'no_reset'.  
    </pre>

  * Unplug the USB-C cable and then plug it back - the new FW should be installed and running.
  * Close the case if it was opened during FW upgrade.
  
**7) Entering first time setup**
----------------------------------

  * Press the service button for ~ 4 seconds, the blue wedge appears on the Clock face
  * On your phone, tablet or PC go to WiFi network settings and scan for a new WiFi.
    A new WiFi AP should appear - the name will be "tickled-ABCD"
    where ABCD will be a unique identifier. Connect to the AP with password "clock-ABCD",
    where ABCD is your unique identifier as found on the WiFi network name. For example if the
    WiFi network name is tickled-15a6 then the password is clock-15a6. You'll be able to change 
    the SSID and the password on the setup Web page.
    
  * If the phone/tablet reports there is no Internet connection, then allow it and keep the connection.
  * open your web browser and enter url: http://192.168.4.1
  * the configuration page should appear
  * at minimum enter the SSID and password to your Internet WiFi network to allow the clock to
    synchronise the time.
  * select the timezone offset, daylight saving offset and the DST rule (if it applies to your country)
  * save the selected options at the bottom of the page by pressing the button "Save Configuration"
  * the board will exit the setup and will try to connect to the Internet WiFi to get the current time



