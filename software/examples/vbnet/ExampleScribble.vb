Imports System
Imports System.Drawing
Imports System.Math
Imports System.Threading
Imports Tinkerforge

Module ExampleScribble
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change to your UID
    Const WIDTH As Integer = 128
    Const HEIGHT As Integer = 64

    Sub DrawImage(ByRef oled As BrickletOLED128x64, ByVal bitmap As Bitmap)
        Dim pages()() As Byte = New Byte(HEIGHT \ 8)() {}

        For row As Integer = 0 To HEIGHT \ 8 - 1
            pages(row) = New Byte(WIDTH) {}

            For column As Integer = 0 To WIDTH - 1
                pages(row)(column) = 0

                For bit As Integer = 0 To 7
                    If bitmap.GetPixel(column, (row * 8) + bit).GetBrightness() > 0 Then
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

        ' Draw rotating line
        Dim bitmap As New Bitmap(WIDTH, HEIGHT)
        Dim originX As Integer = WIDTH \ 2
        Dim originY As Integer = HEIGHT \ 2
        Dim length As Integer = HEIGHT \ 2 - 2
        Dim angle As Integer = 0

        Console.WriteLine("Press enter to exit")

        While Not Console.KeyAvailable
            Dim radians As Double = Math.PI * angle / 180.0
            Dim x As Integer = Convert.ToInt32(Math.Floor(originX + length * Math.Cos(radians)))
            Dim y As Integer = Convert.ToInt32(Math.Floor(originY + length * Math.Sin(radians)))

            Using g As Graphics = Graphics.FromImage(bitmap)
                g.FillRectangle(Brushes.Black, 0, 0, WIDTH, HEIGHT)
                g.DrawLine(Pens.White, originX, originY, x, y)
            End Using

            DrawImage(oled, bitmap)
            Thread.Sleep(25)

            angle += 1
        End While

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
