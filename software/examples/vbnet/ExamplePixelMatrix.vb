Imports System
Imports System.Math
Imports Tinkerforge

Module ExamplePixelMatrix
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change to your UID
    Const WIDTH As Integer = 128
    Const HEIGHT As Integer = 64

    Sub DrawMatrix(ByRef oled As BrickletOLED128x64, ByVal pixels()() As Boolean)
        Dim pages()() As Byte = New Byte(HEIGHT \ 8)() {}

        For row As Integer = 0 To HEIGHT \ 8 - 1
            pages(row) = New Byte(WIDTH) {}

            For column As Integer = 0 To WIDTH - 1
                pages(row)(column) = 0

                For bit As Integer = 0 To 7
                    If pixels((row * 8) + bit)(column) Then
                        pages(row)(column) = pages(row)(column) Or Convert.ToByte(1 << bit)
                    End If
                Next bit
            Next column
        Next row

        oled.NewWindow(0, WIDTH - 1, 0, HEIGHT \ 8 - 1)

        Dim section() As Byte = New Byte(64) {}

        For row As Integer = 0 To HEIGHT \ 8 - 1
            For column As Integer = 0 To WIDTH - 1 Step 64
                Array.Copy(pages(row), column, section, 0, 64)
                oled.Write(section)
            Next row
        Next row
    End Sub

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim oled As New BrickletOLED128x64(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Clear display
        oled.ClearDisplay()

        ' Draw checkerboard pattern
        Dim pixels()() As Boolean = New Boolean(HEIGHT)() {}

        For row As Integer = 0 To HEIGHT - 1
            pixels(row) = New Boolean(WIDTH) {}

            For column As Integer = 0 To WIDTH - 1
                pixels(row)(column) = (row \ 8) Mod 2 = (column \ 8) Mod 2
            Next row
        Next column

        DrawMatrix(oled, pixels)

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
