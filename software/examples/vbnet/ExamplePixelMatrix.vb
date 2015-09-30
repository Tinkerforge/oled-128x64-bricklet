Imports System
Imports Math
Imports Tinkerforge

Module ExamplePixelMatrix
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change to your UID
    Const SCREEN_WIDTH As Integer = 128
    Const SCREEN_HEIGHT As Integer = 64

    Sub DrawMatrix(ByRef oled As BrickletOLED128x64, ByVal pixels()() As Boolean)
        Dim column()() As Byte = New Byte(SCREEN_HEIGHT/8)() {}
        Dim column_write() As Byte = New Byte(64) {}
        Dim page As Byte
        Dim l as Byte

        For i As Integer = 0 To (SCREEN_HEIGHT/8) - 1
            column(i) = New Byte(SCREEN_WIDTH) {}
            For j As Integer = 0 To SCREEN_WIDTH - 1
                page = 0
                For k As Integer = 0 To 7
                    If pixels((i*8) + k)(j) = True Then page = page Or (1 << k)
                Next k
                column(i)(j) = page
            Next j
        Next i

        oled.NewWindow(0, SCREEN_WIDTH-1, 0, 7)

        For i As Integer = 0 To (SCREEN_HEIGHT/8) - 1
            l = 0
            For j As Integer = 0 To (SCREEN_WIDTH/2) - 1
                column_write(l) = column(i)(j)
                l = l + 1
            Next j

            oled.Write(column_write)

            l = 0
            For k As Integer = SCREEN_WIDTH/2 To SCREEN_WIDTH - 1
                column_write(l) = column(i)(k)
                l = l + 1
            Next k

            oled.Write(column_write)
        Next i
    End Sub

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim oled As New BrickletOLED128x64(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Clear display
        oled.ClearDisplay()

        ' Pixel matrix with all pixels turned off
        Dim pixelMatrix()() As Boolean = New Boolean(SCREEN_HEIGHT)() {}

        For i As Integer = 0 To SCREEN_HEIGHT - 1
            pixelMatrix(i) = New Boolean(SCREEN_WIDTH) {}
            For j As Integer = 0 To SCREEN_WIDTH - 1
                pixelMatrix(i)(j) = False
            Next j
        Next i

        ' Draw check pattern
        For w As Integer = 0 To SCREEN_WIDTH - 1
            For h As Integer = 0 To SCREEN_HEIGHT - 1
                If Math.Floor((w/5)) Mod 2 = 0 Then pixelMatrix(h)(w) = True
                If Math.Floor((h/5)) Mod 2 = 0 Then pixelMatrix(h)(w) = True
            Next h
        Next w

        DrawMatrix(oled, pixelMatrix)

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
