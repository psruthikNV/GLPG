# Parses a RenderDoc generated CSV file of clip space coordinates,
# performs a perspective divide generating NDC coorindates and
# a viewport transformation generating screen space coordinates.
#
# The equations used for the conversions are taken from the OpenGL spec.
import csv
import argparse

xindex = 2
yindex = 3
zindex = 4
windex = 5
screenwidth = 1024
screenheight = 768
depthminrange = 0
depthmaxrange = 1
xcenter = screenwidth / 2
ycenter = screenheight / 2
s = (depthmaxrange - depthminrange) / 2
b = (depthmaxrange + depthminrange) / 2
def parse_args():
    parser = argparse.ArgumentParser(description='Parses a CSV file of clip space coordinates and performs a perspective divide')
    parser.add_argument('path', help='Path to a csv file containing clip space coordinates')
    parser.add_argument('--width', type=int, help='Viewport width. Default = 1024')
    parser.add_argument('--height', type=int, help='Viewport height. Default = 768')
    args = parser.parse_args()
    if (args.width != None):
        screenwidth = args.width
    if (args.height != None):
        screenheight = args.height
    return args

if __name__ == "__main__":
    args = parse_args()
    with open(args.path, newline='') as csvfile:
        # Skip the first line containing strings
        next(csvfile)
        clipcoordreader = csv.reader(csvfile, quotechar='|', quoting=csv.QUOTE_NONNUMERIC)
        for row in clipcoordreader:
            # Remove unwanted rows.
            del row[-1]
            del row[-1]
            row[xindex] = row[xindex] / row[windex]
            row[yindex] = row[yindex] / row[windex]
            row[zindex] = row[zindex] / row[windex]
            row[windex] = row[windex] / row[windex]
            row.append(((screenwidth * row[xindex]) / 2) + xcenter)
            row.append(((screenheight * row[yindex]) / 2) + ycenter)
            row.append((s * row[zindex]) + b)
            print(' ',row)

