// import fs from "fs";


// const fd = fs.openSync("/dev/input/js0", "r");
// const buffer = Buffer.alloc(8);


// export let state: { x: number, y: number, angle?: number, led?: number, ledNext?: number, ledPrev?: number, travel?: number } = {
//     x: 0,
//     y: 0,
//     angle: 0,
// };

// function stickToAngle(x: number, y: number) {
//     // deadzone (optional but recommended)
//     const deadzone = 2000;
//     if (Math.abs(x) < deadzone && Math.abs(y) < deadzone) {
//         return; // stick centered
//     }

//     let angle = Math.atan2(y, x) * (180 / Math.PI);

//     // convert from [-180, 180] → [0, 360)
//     if (angle < 0) angle += 360;

//     return Math.floor(angle);
// }

// function angleToLed(angle?: number) {
//     if (!angle && angle !== 0) return;
//     const leds = 24;
//     const step = 360 / leds; // 15°

//     return Math.floor(angle / step) % leds;
// }


// export function start() {

//     const raw = fs.readFileSync("/proc/bus/input/devices", "utf8");

//     // console.log(raw);

//     function readNext() {
//         fs.read(fd, buffer, 0, 8, null, (err, bytesRead) => {
//             if (err) {
//                 // console.error(err);
//                 setImmediate(readNext);
//                 return;
//             }

//             if (bytesRead === 8) {
//                 const value = buffer.readInt16LE(4);
//                 const type = buffer.readUInt8(6);
//                 const number = buffer.readUInt8(7);
//                 // console.log(number, value, type);
//                 const JS_EVENT_AXIS = 0x02;

//                 if (type & JS_EVENT_AXIS) {
//                     if (number === 0) state.x = value;
//                     if (number === 1) state.y = value;

//                     state.angle = stickToAngle(state.x, state.y);
//                     state.led = angleToLed(state.angle);
//                     state.ledNext = state.led || state.led === 0 ? (state.led + 1) % 24 : undefined;
//                     state.ledPrev = state.led || state.led === 0 ? (state.led - 1 + 24) % 24 : undefined;

//                     // console.log({ x: state.x, y: state.y });
//                     state.travel = scaleInt16to8(Math.abs(state.x) + Math.abs(state.y))
//                     // console.log(state.travel);
//                 }
//             }

//             readNext();
//         });
//     }

//     readNext();
// }

// function scaleInt16to8(value: number) {
//     return ((value) * 255 / 65534) | 0;
// }

// export function calculateRingColors() {
//     const ringColors = new Array(24).fill({
//         r: 0,
//         g: 0,
//         b: 1
//     });
//     if (state.led || state.led === 0) {
//         ringColors[state.led] = {
//             r: state.travel,
//             g: 0,
//             b: 0,
//         }
//     }
//     if (state.ledNext || state.ledNext === 0) {
//         ringColors[state.ledNext] = {
//             r: 50,
//             g: 0,
//             b: 100
//         }
//     }
//     if (state.ledPrev || state.ledPrev === 0) {
//         ringColors[state.ledPrev] = {
//             r: 50,
//             g: 0,
//             b: 100
//         }
//     }


//     return ringColors;
// }