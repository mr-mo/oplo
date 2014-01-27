
template<typename ClockType, typename OutputPrecision>
ChonoStopwatch<ClockType, OutputPrecision>::ChonoStopwatch()
{}

template<typename ClockType, typename OutputPrecision>
void ChonoStopwatch<ClockType, OutputPrecision>::start()
{
	m_start = ClockType::now();
}

template<typename ClockType, typename OutputPrecision>
void ChonoStopwatch<ClockType, OutputPrecision>::stop()
{
	m_stop = ClockType::now();
}

template<typename ClockType, typename OutputPrecision>
typename ChonoStopwatch<ClockType, OutputPrecision>::ReturnType ChonoStopwatch<ClockType, OutputPrecision>::runningDiff() const
{
	return std::chrono::duration_cast<OutputPrecision>(ClockType::now() - m_start).count();
}

template<typename ClockType, typename OutputPrecision>
typename ChonoStopwatch<ClockType, OutputPrecision>::ReturnType ChonoStopwatch<ClockType, OutputPrecision>::diff() const
{
	return std::chrono::duration_cast<OutputPrecision>(m_stop - m_start).count();
}

template<typename ClockType, typename OutputPrecision>
typename ChonoStopwatch<ClockType, OutputPrecision>::ReturnType ChonoStopwatch<ClockType, OutputPrecision>::poll()
{
	return std::chrono::duration_cast<OutputPrecision>(ClockType::now().time_since_epoch()).count();
}
