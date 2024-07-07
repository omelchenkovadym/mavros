# Indicators of radio signal search results
#
# RADIO_SIGNAL message

std_msgs/Header header

float32 rate	# signal rate Hz 433..1020
int16 heading	# signal direction degrees 0..180
int16 level	    # signal level dB -140..0

