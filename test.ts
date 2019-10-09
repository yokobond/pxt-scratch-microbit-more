// tests go here; this will not be compiled when this package is used as a library

ScratchMore.startScratchMoreService(() => {
  // ScratchMore.setScratchMoreSlot(Slot.SLOT0, input.magneticForce(Dimension.Strength))
  ScratchMore.setScratchMoreSlot(Slot.SLOT1, input.acceleration(Dimension.X))
  ScratchMore.setScratchMoreSlot(Slot.SLOT2, input.acceleration(Dimension.Y))
  ScratchMore.setScratchMoreSlot(Slot.SLOT3, input.acceleration(Dimension.Z))
});

input.onButtonPressed(Button.A, function () {
  basic.showNumber(ScratchMore.getScratchMoreSlot(0))
})