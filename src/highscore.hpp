/*
    Copyright 2000, 2001, 2002, 2003 Slingshot Game Technology, Inc.

    This file is part of The Soul Ride Engine, see http://soulride.com

    The Soul Ride Engine is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2 of
    the License, or (at your option) any later version.

    The Soul Ride Engine is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
// highscore.hpp	-thatcher 9/26/2000 Copyright Slingshot Game Technology

// Module for maintaining local and world-wide high-score tables.


#ifndef HIGHSCORE_HPP
#define HIGHSCORE_HPP


namespace HighScore {
	void	Open();
	void	Close();
	void	Clear();

	void	Read();

	void	RegisterScore(const char* PlayerName, int run, int score);

	int	GetLocalHighScore(int run, int rank = 1);
	const char*	GetLocalHighPlayer(int run, int rank = 1);

	int	GetWorldHighScore(int run, int rank = 1);
	const char*	GetWorldHighPlayer(int run, int rank = 1);
};


#endif // HIGHSCORE_HPP

