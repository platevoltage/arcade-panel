import { SerialPort } from "serialport";
import { delay } from "./utils.ts";
import * as sticks from "./sticks.ts";

const NUM_BUTTONS = 20;
const OVER_SCAN = 2;

let runId = 0;
export async function sendButtonColors(port: SerialPort, colors: string[]) {

    if (colors.length <= (NUM_BUTTONS / 2)) {
        colors = [...colors, ...colors];
    }
    colors.splice(NUM_BUTTONS);
    // console.log(colors);

    const player1Top = [ //
        "#FF0000", "#FF0000", "#FF0000", "#FF0000"];
    const player1Bottom = [ //
        "#FF0000", "#FF0000", "#FF0000", "#FF0000"];
    const p1StartSelect = colors.slice(8, (NUM_BUTTONS / 2));
    const player2Top = [ //
        "#0000FF", "#0000FF", "#0000FF", "#0000FF"];
    const player2Bottom = [ //
        "#0000FF", "#0000FF", "#0000FF", "#0000FF"];
    const p2StartSelect = colors.slice(18, NUM_BUTTONS);

    const myRun = ++runId;
    let finalColorsInt: number[] = [];

    for (let i = -OVER_SCAN; (i < NUM_BUTTONS + OVER_SCAN * 2); i++) {

        player1Top[i] = colors.slice(0, 4)[i];
        player1Bottom[i - 1] = colors.slice(4, 8)[i - 1];
        player2Top[i - 14] = colors.slice(10, 14)[i - 14];
        player2Bottom[i - 15] = colors.slice(14, 18)[i - 15];

        const _player1Top = [...player1Top];
        const _player1Bottom = [...player1Bottom];
        const _player2Top = [...player2Top];
        const _player2Bottom = [...player2Bottom];
        _player1Top[i + 1] = "#FFFFFF";
        _player1Bottom[i] = "#FFFFFF";
        _player1Top[i + 2] = "#FFFFFF";
        _player1Bottom[i + 1] = "#FFFFFF";

        _player2Top[i - 13] = "#FFFFFF";
        _player2Bottom[i - 14] = "#FFFFFF";
        _player2Top[i - 12] = "#FFFFFF";
        _player2Bottom[i - 13] = "#FFFFFF";

        const finalColors = [ //
            ..._player1Top.slice(0, 4),
            ..._player1Bottom.slice(0, 4),
            ...p1StartSelect,
            ..._player2Top.slice(0, 4),
            ..._player2Bottom.slice(0, 4),
            ...p2StartSelect
        ];

        finalColorsInt = finalColors.map(c => parseInt(c.replace("#", "0x")));

        const json = { "buttons": finalColorsInt.map(c => scaleBrightness(c, 1)) };
        // console.log(json);
        port.write(JSON.stringify(json) + "\n");
        await delay(50);
    }
    await delay(500);

    let count = 0;
    let forward = true;
    while (true) {
        //do other shit with buttons. fades, etc here.
        if (myRun !== runId) return;

        forward ? count++ : count--;
        if (count >= 70) {
            forward = false;
        } else if (count <= 0) {
            forward = true;
            // await delay(1000);
        }
        // const stickCoords = sticks.state;
        // console.log(stickCoords);
        // const ringColors = [];
        // for (let i = 0; i < 24; i++) {
        //     if (stickCoords?.led === i) {
        //         ringColors.push({
        //             r: 100,
        //             g: 50,
        //             b: i * 3
        //         })
        //     } else {
        //         ringColors.push({
        //             r: 0,
        //             g: 0,
        //             b: 0
        //         })
        //     }
        // }
        const json = { "buttons": finalColorsInt.map(c => scaleBrightness(c, 1 * (1 - count * .013))), "sticks": sticks.calculateRingColors() };
        // console.log(json);
        port.write(JSON.stringify(json) + "\n");
        await delay(25);
    }

}

function scaleBrightness(color: number, factor: number) {
    let r = ((color >> 16) & 0xFF) * factor * 10;
    let g = ((color >> 8) & 0xFF) * factor * 10;
    let b = (color & 0xFF) * factor * 10;

    return {
        r: Math.round(r),
        g: Math.round(g),
        b: Math.round(b)
    };
}
