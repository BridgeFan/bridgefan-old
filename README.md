A simple card game Blef (in English Bluff).
The code was written in 2017.
The game is written in C++ using SFML and needs to be compiled using Visual Studio.
The game supports playing with single or multiple players and has a simple computer player.
It chooses most propable option considering only cards known by the computer and assuming that rest of cards are random.

Rules of the game:
Each player begins with the same number of cards. After each deal the losing player plays with one more card for each time he or she lost. If he or she gets maximum number of cards, ends the game and loses. Game is played until only one player is still playing.
In each deal players, beginning from the one losing the previous game, call exact poker hands (like pair of aces or three kings). Next call has to be higher than the previous one (for example, both one ace and two nines are higher than one king). Player can also check the previous call. If that hand is present in pool of cards of all players, checking player loses the deal. Otherwise, player who made the last call loses the deal.
