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



**3) soldering of electronics**
---------------------------------

more to come ...
   
   
      
  
  


