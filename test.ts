// tests go here; this will not be compiled when this package is used as a library

MbitMore.startMbitMoreService(() => {
  // MbitMore.setMbitMoreSharedData(SharedDataIndex.DATA0, input.magneticForce(Dimension.Strength))
  MbitMore.setMbitMoreSharedData(SharedDataIndex.DATA1, input.acceleration(Dimension.X))
  MbitMore.setMbitMoreSharedData(SharedDataIndex.DATA2, input.acceleration(Dimension.Y))
  MbitMore.setMbitMoreSharedData(SharedDataIndex.DATA3, input.acceleration(Dimension.Z))
});

input.onButtonPressed(Button.A, function () {
  basic.showNumber(MbitMore.getMbitMoreSharedData(0))
})