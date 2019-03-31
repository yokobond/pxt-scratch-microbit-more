// tests go here; this will not be compiled when this package is used as a library

bluetooth.startScratchMoreService(() => {
  led.toggle(0,0);
  bluetooth.setScratchMoreSlot(0, input.lightLevel());
  bluetooth.setScratchMoreSlot(1, input.compassHeading());
  bluetooth.setScratchMoreSlot(2, input.rotation(Rotation.Pitch));
  bluetooth.setScratchMoreSlot(3, input.rotation(Rotation.Roll));
});
