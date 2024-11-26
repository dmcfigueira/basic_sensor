# ********************************************************************************
# 
# Set of tests used to validate the usage of a ring buffer to store data.
#
# Created on Sat Nov 23 2024
#
# Daniel Figueira <daniel.castro.figueira@gmail.com>
# 
# ********************************************************************************
import test_utils.usb_comm as usb

##################### Constants ######################

RING_BUFFER_SIZE = 10

##################### Test Cases #####################

class TestRingBuffer:
    @classmethod
    def setup_class(cls):
        usb.init()

    @classmethod
    def teardown_class(cls):
        pass

    def setup_method(self):
        usb.clear_buffers()
        pass

    def teardown_method(self):
        usb.set_default_data_rates()

    def test_1_1_ReducedRate_NoDroppedSamples_WhenNSamplesIsSmallerOrEqualToBufferSize(self):
        ''' No samples are dropped when data is sent at a reduced rate as long
            as the number of samples queued is smaller or equal to the buffer size '''
        usb.set_data_rate(1000)  
        usb.set_read_rate(1000)
        usb.set_send_rate(10) # slow send rate 
        data = usb.simulate_increasing_pattern(0, 1, RING_BUFFER_SIZE - 1)
        assert data == [i for i in range(RING_BUFFER_SIZE)]
        
    def test_1_2_ReducedRate_DroppedSamples_WhenNSamplesIsBiggerThanBufferSize(self):
        ''' Samples are dropped when data is sent at a reduced rate and the number
            of samples queued surpasses the buffer size '''
        usb.set_data_rate(1000)  
        usb.set_read_rate(1000)
        usb.set_send_rate(10) # slow send rate
        data = usb.simulate_increasing_pattern(0, 1, 2 * RING_BUFFER_SIZE - 1)
        assert RING_BUFFER_SIZE <= len(data) <= RING_BUFFER_SIZE + 1
        assert data[-RING_BUFFER_SIZE:] == [RING_BUFFER_SIZE + i for i in range(RING_BUFFER_SIZE)]

    def test_2_1_IncreasedRate_NoDuplicates(self):
        ''' Missed samples happen when the data rate is bigger than the read rate '''
        usb.set_data_rate(10)
        usb.set_read_rate(10)
        usb.set_send_rate(1000)
        assert usb.simulate_increasing_pattern(0, 1, 10) == [i for i in range(10 + 1)]
    
