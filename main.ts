let state = 0
function onDisconnected() {
    state = 0
    while (state == 0) {
        basic.showString(control.deviceName())
    }
}
ScratchMore.startScratchMoreService(function () {

})
bluetooth.onBluetoothConnected(function () {
    state = 1
})
bluetooth.onBluetoothDisconnected(function () {
    onDisconnected()
})
onDisconnected()
