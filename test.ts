// tests go here; this will not be compiled when this package is used as a library

ScratchMore.startScratchMoreService(() => {
  led.toggle(0,0);
  ScratchMore.setScratchMoreSlot(Slot.SLOT0, input.lightLevel());
  ScratchMore.setScratchMoreSlot(Slot.SLOT1, input.compassHeading());
  ScratchMore.setScratchMoreSlot(Slot.SLOT2, input.rotation(Rotation.Pitch));
  ScratchMore.setScratchMoreSlot(Slot.SLOT3, input.rotation(Rotation.Roll));
});
