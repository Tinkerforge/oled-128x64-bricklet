#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import functools

from PyQt5.QtCore import Qt
from PyQt5.QtGui import QPixmap, QColor, QPainter, QFont
from PyQt5.QtWidgets import QApplication, QWidget, QComboBox, QToolButton, \
                            QHBoxLayout, QVBoxLayout, QPushButton

def toggle_pixel(x, y, font, combo_index, button_pixel):
    index = combo_index.currentIndex()
    glyph = font[index]
    glyph[x][y] = not glyph[x][y]

    button_pixel.setStyleSheet('background-color: red' if glyph[x][y] else '')

def change_pixels(index, font, button_pixels):
    glyph = font[index]

    for x in range(5):
        column = glyph[x]

        for y in range(8):
            button_pixels[x][y].setStyleSheet('background-color: red' if column[y] else '')

def fill_pixels(font, combo_index, button_pixels, value):
    index = combo_index.currentIndex()
    glyph = font[index]

    for x in range(5):
        column = glyph[x]

        for y in range(8):
            column[y] = value

    change_pixels(index, font, button_pixels)

def load_font_inc():
    with open('font.inc', 'r') as f:
        lines = f.readlines()

    font = []

    for line in lines:
        if not line.startswith('\t'):
            continue

        glyph = []

        for cs in line.strip().split(',')[:5]:
            cs = cs.strip().split(' ')[0]
            ci = int(cs, base=16)
            column = []

            for bit in range(8):
                column.append(ci & (1 << bit))

            glyph.append(column)

        font.append(glyph)

    return font

def save_font_inc(font):
    rows = []

    for glyph in font:
        values = []

        for column in glyph:
            value = 0

            for bit in range(8):
                if column[bit]:
                    value |= 1 << bit

            values.append('0x{:02X}'.format(value))

        rows.append(', '.join(values))

    with open('font.inc', 'w') as f:
        f.write('// Standard ASCII 5x7 font\n\nconst uint8_t font[] = {\n\t' + ',\n\t'.join(rows) + '\n};\n')

def save_font_png(font):
    # global border
    Bl = 10
    Br = 10
    Bt = 7
    Bb = 7

    # border
    bl = 7
    br = 7
    bt = 27
    bb = 10

    # pixel size
    pw = 4
    ph = 4

    # char spacing
    sw = 6
    sh = 20

    # grid thickness
    gw = 1
    gh = 1

    # text offset
    ch = -22

    pixmap = QPixmap(Bl + (bl + (pw + gw) * 5 - gw + br) * 16 + Br,
                     Bt + (bt + (ph + gh) * 8 - gh + bb) * 16 + Bb)
    painter = QPainter(pixmap)
    label_font = QFont('Courier New', 12)

    label_font.setBold(True)

    painter.setPen(Qt.black)
    painter.setFont(label_font)
    painter.fillRect(0, 0, pixmap.width(), pixmap.height(), Qt.white)

    xb = Bl + bl
    yb = Bt + bt
    counter = 0

    for glyph in font:
        if counter > 31 and counter < 127:
            text = chr(counter)
            background = QColor(200, 255, 200)
        else:
            text = (u'%02X' % counter) + u'\u2095'
            background = Qt.white

        if counter > 0:
            painter.fillRect(xb - bl, yb - bt, bl + (pw + gw) * 5 - gw + br, bt + (ph + gh) * 8 - gh + bb, background)
            painter.drawText(xb, yb + ch, (pw + gw) * 5 - gw, 100, Qt.AlignHCenter | Qt.TextDontClip, text)

            xo = 0

            for column in glyph:
                for bit, pixel in enumerate(column):
                    painter.fillRect(xb + xo, yb + (ph + gh) * bit, pw, ph, Qt.black if pixel else Qt.lightGray)

                xo += pw + gw

        yb += (ph + gh) * 8 - gh + bb + bt
        counter += 1

        if counter % 16 == 0:
            xb += (pw + gw) * 5 - gw + br + bl
            yb = Bt + bt

    painter.end()
    pixmap.save('font.png', 'PNG')

def main():
    app = QApplication(sys.argv + ['-style', 'fusion'])
    font = load_font_inc()
    widget = QWidget()
    combo_index = QComboBox()

    for c in range(256):
        if chr(c).isprintable() and c < 128:
            combo_index.addItem('{0}: {1}'.format(c, chr(c)))
        else:
            combo_index.addItem('{0}: {0:02X}h'.format(c))

    button_pixels = []
    layout_pixels = QHBoxLayout()
    button_fill = QPushButton('Fill')
    button_clear = QPushButton('Clear')
    button_save = QPushButton('Save')

    layout_main = QVBoxLayout(widget)
    layout_main.addWidget(combo_index)
    layout_main.addLayout(layout_pixels)
    layout_main.addWidget(button_fill)
    layout_main.addWidget(button_clear)
    layout_main.addWidget(button_save)

    for x in range(5):
        column = []
        layout_column = QVBoxLayout()

        for y in range(8):
            button_pixel = QToolButton()
            button_pixel.setText('   ')
            button_pixel.clicked.connect(functools.partial(toggle_pixel, x, y, font, combo_index, button_pixel))

            column.append(button_pixel)
            layout_column.addWidget(button_pixel)

        button_pixels.append(column)
        layout_pixels.addLayout(layout_column)

    combo_index.currentIndexChanged.connect(lambda index: change_pixels(index, font, button_pixels))
    button_fill.clicked.connect(lambda: fill_pixels(font, combo_index, button_pixels, True))
    button_clear.clicked.connect(lambda: fill_pixels(font, combo_index, button_pixels, False))
    button_save.clicked.connect(lambda: save_font_png(font))
    button_save.clicked.connect(lambda: save_font_inc(font))

    widget.show()

    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
