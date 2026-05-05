#!/usr/bin/env python3

import re
import sys
import os

HEX_LINE_RE = re.compile(
    r'^([0-9A-Fa-f]{6}):\s*((?:[0-9A-Fa-f]{2}(?:\s+|$))+)$'
)


def parse_hex_bytes(data_str):
    vals = data_str.strip().split()
    return bytes(int(x, 16) for x in vals)


def write_packet(packet_num, data, outdir):
    filename = f"{packet_num:04d}"
    path = os.path.join(outdir, filename)

    with open(path, "wb") as out:
        out.write(data)

    print(f"Wrote {path} ({len(data)} bytes)", file=sys.stderr)


def extract_packets(stream, outdir):
    packet_num = 0

    current_data = bytearray()
    expected_offset = None
    in_packet = False

    for line in stream:
        line = line.rstrip("\n")

        m = HEX_LINE_RE.match(line)

        if m:
            offset = int(m.group(1), 16)
            hexbytes = parse_hex_bytes(m.group(2))

            if not in_packet:
                if offset == 0:
                    current_data = bytearray(hexbytes)
                    expected_offset = len(hexbytes)
                    in_packet = True
                else:
                    continue
            else:
                if offset == expected_offset:
                    current_data.extend(hexbytes)
                    expected_offset += len(hexbytes)
                else:
                    if current_data:
                        write_packet(packet_num, current_data, outdir)
                        packet_num += 1

                    if offset == 0:
                        current_data = bytearray(hexbytes)
                        expected_offset = len(hexbytes)
                        in_packet = True
                    else:
                        current_data = bytearray()
                        expected_offset = None
                        in_packet = False

        else:
            if in_packet and current_data:
                write_packet(packet_num, current_data, outdir)
                packet_num += 1

            current_data = bytearray()
            expected_offset = None
            in_packet = False

    if in_packet and current_data:
        write_packet(packet_num, current_data, outdir)


def main():
    outdir = "packets"
    os.makedirs(outdir, exist_ok=True)

    extract_packets(sys.stdin, outdir)


if __name__ == "__main__":
    main()
