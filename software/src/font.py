#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from PyQt4.QtCore import Qt
from PyQt4.QtGui import QApplication, QPixmap, QPainter, QFont, QColor

if __name__ == '__main__':
    app = QApplication(sys.argv)

    with open('font.inc', 'rb') as f:
        lines = f.readlines()

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
    font = QFont('Courier New', 12)

    font.setBold(True)

    painter.setPen(Qt.black)
    painter.setFont(font)
    painter.fillRect(0, 0, pixmap.width(), pixmap.height(), Qt.white)

    xb = Bl + bl
    yb = Bt + bt
    counter = 0

    for line in lines:
        if not line.startswith('\t'):
            continue

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

            for cs in line.strip().split(',')[:5]:
                cs = cs.strip().split(' ')[0]
                ci = int(cs, base=16)

                for bit in range(8):
                    if ci & (1 << bit) != 0:
                        painter.fillRect(xb + xo, yb + (ph + gh) * bit, pw, ph, Qt.black)
                    else:
                        painter.fillRect(xb + xo, yb + (ph + gh) * bit, pw, ph, Qt.lightGray)

                xo += pw + gw

        yb += (ph + gh) * 8 - gh + bb + bt
        counter += 1

        if counter % 16 == 0:
            xb += (pw + gw) * 5 - gw + br + bl
            yb = Bt + bt

    painter.end()
    pixmap.save('font.png', 'PNG')
