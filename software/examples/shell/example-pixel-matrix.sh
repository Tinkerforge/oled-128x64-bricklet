#!/bin/bash
# connects to localhost:4223 by default, use --host and --port to change it

# This example requires Bash 4

# change to your UID
uid=XYZ

SCREEN_WIDTH=128
SCREEN_HEIGHT=64

function draw_matrix {
	declare -A column
	declare -A column_write

	for ((i=0; i<${SCREEN_HEIGHT}/8; i++))
	do
		for ((j=0; j<${SCREEN_WIDTH}; j++))
		do
			page=0

			for ((k=0; k<8; k++))
			do
				if ((${pixel_matrix[$((((${i}*8))+${k})),${j}]}))
				then
					page=$((${page}|$((1<<${k}))))
				fi
			done
			column[${i},${j}]=${page}
		done
	done
	tinkerforge call oled-128x64-bricklet ${uid} new-window 0 $((${SCREEN_WIDTH}-1)) 0 7

	for ((i=0; i<${SCREEN_HEIGHT}/8; i++))
	do
		l=0
		for ((j=0; j < ${SCREEN_WIDTH}/2; j++))
		do
			column_write[${l}]=${column[${i},${j}]}
			l=$((l+1))
		done
		write_bytes=""
		for ((j=0; j<${SCREEN_WIDTH}/2; j++))
		do
			write_bytes+=${column_write[${j}]}
			if ((${j}==${SCREEN_HEIGHT}-1))
			then
				continue
			fi
			write_bytes+=","
		done
		tinkerforge call oled-128x64-bricklet ${uid} write ${write_bytes}

		l=0
		for ((j=${SCREEN_WIDTH}/2; j < ${SCREEN_WIDTH}; j++))
		do
			column_write[${l}]=${column[${i},${j}]}
			l=$((l+1))
		done
		write_bytes=""
		for ((j=0; j<${SCREEN_WIDTH}/2; j++))
		do
			write_bytes+=${column_write[${j}]}
			if ((${j}==${SCREEN_HEIGHT}-1))
			then
				continue
			fi
			write_bytes+=","
		done
		tinkerforge call oled-128x64-bricklet ${uid} write ${write_bytes}
	done
}

# clear display
tinkerforge call oled-128x64-bricklet ${uid} clear-display

# Pixel matrix with all pixels turned off
declare -A pixel_matrix

for ((h=0;h<${SCREEN_HEIGHT};h++))
do
	for ((w=0;w<${SCREEN_WIDTH};w++))
	do
		pixel_matrix[${h},${w}]=0
	done
done

# Draw check pattern
for ((w=0; w<${SCREEN_WIDTH}; w++))
do
	for ((h=0; h<${SCREEN_HEIGHT}; h++))
	do
		if !((((${w}/5))%2))
		then
			pixel_matrix[${h},${w}]=1
		fi
		if !((((${h}/5))%2))
		then
			pixel_matrix[${h},${w}]=1
		fi
	done
done

draw_matrix
