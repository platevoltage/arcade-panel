import { spawn } from "child_process";
import fs from "fs";

// export default function startStick() {


const fd = fs.openSync("/dev/input/js0", "r");

const buffer = Buffer.alloc(8);

// state tracking
let state = {
    x: 0,
    y: 0
};

// js_event struct (Linux joystick API)
// struct js_event {
//   uint32_t time;
//   int16_t value;
//   uint8_t type;
//   uint8_t number;
// };
function stickToAngle(x: number, y: number) {
    // deadzone (optional but recommended)
    const deadzone = 2000;
    if (Math.abs(x) < deadzone && Math.abs(y) < deadzone) {
        return null; // stick centered
    }

    let angle = Math.atan2(y, x) * (180 / Math.PI);

    // convert from [-180, 180] → [0, 360)
    if (angle < 0) angle += 360;

    return Math.floor(angle);
}

function angleToLed(angle: number | null) {
    if (!angle && angle !== 0) return;
    const leds = 24;
    const step = 360 / leds; // 15°

    return Math.floor(angle / step) % leds;
}

export function pollStick() {
    setTimeout(() => {
        const angle = stickToAngle(state.x, state.y);
        const led = angleToLed(angle);
        return { x: state.x, y: state.y, angle, led };
    }, 500);
    fs.readSync(fd, buffer, 0, 8, null);

    const time = buffer.readUInt32LE(0);
    const value = buffer.readInt16LE(4);
    const type = buffer.readUInt8(6);
    const number = buffer.readUInt8(7);

    const JS_EVENT_AXIS = 0x02;

    if (type === JS_EVENT_AXIS) {
        // typical mapping (may vary slightly by controller)
        if (number === 0) state.x = value;
        if (number === 1) state.y = value;

    }
    // console.log(`X=${state.x} Y=${state.y}`);
    const angle = stickToAngle(state.x, state.y);
    const led = angleToLed(angle);
    return { x: state.x, y: state.y, angle, led };
}

// const device = "/dev/input/js0";
// console.log(device);
// // state cache so we only log changes
// let state = {
//     x: 0,
//     y: 0
// };

// const proc = spawn("jstest", [device], {
//     stdio: ["ignore", "pipe", "pipe"]
// });

// function parseLine(line: string) {
//     // jstest output varies slightly, but usually contains lines like:
//     // Axis 0:  -1234
//     // Axis 1:   5678

//     const match = line.match(/Axis\s+([0-9]+):\s+(-?\d+)/);
//     if (!match) return;

//     const axis = parseInt(match[1], 10);
//     const value = parseInt(match[2], 10);

//     // typical mapping:
//     // 0 = X, 1 = Y (left stick)
//     if (axis === 0) state.x = value;
//     if (axis === 1) state.y = value;

//     console.log(`X=${state.x} Y=${state.y}`);
// }

// let buffer = "";

// proc.stdout.on("data", (chunk) => {
//     console.log(chunk.toString());
//     buffer += chunk.toString();

//     const lines = buffer.split("\n");
//     buffer = lines.pop() ?? ""; // keep partial line

//     for (const line of lines) {
//         parseLine(line.trim());
//     }
// });

// proc.stderr.on("data", (data) => {
//     // optional: uncomment for debugging
//     console.error(data.toString());
// });

// proc.on("exit", (code) => {
//     console.log("jstest exited:", code);
// });
// }