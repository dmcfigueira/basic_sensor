# ********************************************************************************
# 
# Set of tests used to validate the simulation of sensor data.
#
# Created on Sat Nov 23 2024
#
# Daniel Figueira <daniel.castro.figueira@gmail.com>
# 
# ********************************************************************************
import test_utils.usb_comm as usb

##################### Test Cases #####################

class TestSensorSimulation:
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

    def test_1_1_Simulation_NoData_WhenIdle(self):
        ''' No data is received when the sensor is idle '''
        assert not usb.read_data()

    def test_1_2_Simulation_NoData_AfterSimulation(self):
        ''' No data is received after a simulation is executed '''
        assert usb.simulate_const_pattern(10, 5)
        assert not usb.read_data()

    def test_2_1_ConstPattern_DataIsOk(self): 
        ''' Data is correctly simulated for a constant pattern '''
        assert usb.simulate_const_pattern(10, 5) == [10.0, 10.0, 10.0, 10.0, 10.0]

    def test_2_2_ConstPattern_NoData_WhenNSamplesIsZero(self):
        ''' No data is received when the number of samples is zero '''
        assert not usb.simulate_const_pattern(10, 0)

    def test_3_1_IncreasingPattern_DataIsOk(self):
        ''' Data is correctly simulated for an increasing pattern '''
        assert usb.simulate_increasing_pattern(10, 2, 20) == [10.0, 12.0, 14.0, 16.0, 18.0, 20.0]

    def test_3_2_IncreasingPattern_NoData_WhenIntervalIsInvalid(self):
        ''' No data is received when the interval is invalid '''
        assert not usb.simulate_increasing_pattern(20, 2, 10)

    def test_4_1_DecreasingPattern_DataIsOk(self):
        ''' Data is correctly simulated for a decreasing pattern '''
        assert usb.simulate_decreasing_pattern(20, 2, 10) == [20.0, 18.0, 16.0, 14.0, 12.0, 10.0]

    def test_4_2_IncreasingPattern_NoData_WhenIntervalIsInvalid(self):
        ''' No data is received when the interval is invalid '''
        assert not usb.simulate_decreasing_pattern(10, 2, 20)
    
    def test_5_1_RandomPattern_DataIsOk(self): 
        ''' Data is correctly simulated for a random pattern '''
        data = usb.simulate_random_pattern(10, 20, 5)
        assert len(data) == 5
        assert all([10 <= x <= 20 for x in data])

    def test_5_2_RandomPattern_NoData_WhenIntervalIsInvalid(self): 
        ''' No data is received when the interval is invalid '''
        assert not usb.simulate_random_pattern(20, 10, 5)

    def test_6_1_DataRate_DataIsOk_At10Hz(self):
        ''' Data is correctly simulated at 10Hz '''
        usb.set_data_rate(10)
        usb.set_read_rate(10)
        usb.set_send_rate(10)
        data = usb.simulate_increasing_pattern(0, 1, 10)
        assert data == [i for i in range(10 + 1)]

    def test_6_2_DataRate_DataIsOk_At100Hz(self):
        ''' Data is correctly simulated at 100Hz '''
        usb.set_data_rate(100)
        usb.set_read_rate(100)
        usb.set_send_rate(100)
        data = usb.simulate_increasing_pattern(0, 1, 100)
        assert data == [i for i in range(100 + 1)]
        
    def test_6_3_DataRate_NumSamplesIsOk_At1000Hz(self):
        ''' Data is correctly simulated at 1000Hz '''
        usb.set_data_rate(1000)
        usb.set_read_rate(1000)
        usb.set_send_rate(1000)
        data = usb.simulate_increasing_pattern(0, 1, 1000)
        assert data == [i for i in range(1000 + 1)]

    def test_6_4_DataRate_Duplicates_WhenDataRateIsLesserThanReadRate(self):
        ''' Duplicates happen when the data rate is lesser than the read rate '''
        usb.set_data_rate(10)
        usb.set_read_rate(20)
        usb.set_send_rate(20)
        assert usb.simulate_increasing_pattern(10, 2, 20) == [10.0, 10.0, 12.0, 12.0, 14.0, 14.0, 16.0, 16.0, 18.0, 18.0, 20.0, 20.0]
        pass

    def test_6_5_DataRate_MissedSamples_WhenDataRateIsBiggerThanReadRate(self):
        ''' Missed samples happen when the data rate is bigger than the read rate '''
        usb.set_data_rate(20)
        usb.set_read_rate(10)
        usb.set_send_rate(10)
        assert usb.simulate_increasing_pattern(10, 2, 20) == [10.0, 14.0, 18.0]
        pass
    
    
