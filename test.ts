// tests go here; this will not be compiled when this package is used as a library

MbitMore.startMbitMoreService(() => {
  // MbitMore.setMbitMoreSlot(Slot.SLOT0, input.magneticForce(Dimension.Strength))
  MbitMore.setMbitMoreSlot(Slot.SLOT1, input.acceleration(Dimension.X))
  MbitMore.setMbitMoreSlot(Slot.SLOT2, input.acceleration(Dimension.Y))
  MbitMore.setMbitMoreSlot(Slot.SLOT3, input.acceleration(Dimension.Z))
});

input.onButtonPressed(Button.A, function () {
  basic.showNumber(MbitMore.getMbitMoreSlot(0))
})