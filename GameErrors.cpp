#include "GameErrors.h"

namespace game
{
	GameError::operator bool() const
	{
		return  (lastErrorType != GameErrors::None);
	}
}