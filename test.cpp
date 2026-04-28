#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>

// _WIN32 Libraries
#include <windows.h>
#include <conio.h> // _kbhit(), _getch()

using namespace std;

// Constants
const string CSV_FILE = "playlist.csv";
const string EXPECTED_HEADER = "Title,Artist,Genre,Year,Duration";
const string PLAYLIST_INDEX_FILE = "playlists.txt"; // Stores list of saved playlist names

// Clear Screen
void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Sleep Function
void sleepOneSecond()
{
#ifdef _WIN32
    Sleep(1000);
#else
    sleep(1);
#endif
}

// Helper Function
string toLower(string str)
{
    for (int i = 0; i < (int)str.length(); i++)
        str[i] = tolower(str[i]);
    return str;
}

// CLASS: Song
class Song
{
public:
    string title;
    string artist;
    string genre;
    int year;
    string duration; // "MM:SS"

    Song() { year = 0; }

    Song(string t, string a, string g, int y, string d)
    {
        title = t;
        artist = a;
        genre = g;
        year = y;
        duration = d;
    }

    // Convert "MM:SS" → total seconds
    int durationInSeconds() const
    {
        int colonPos = duration.find(":");
        if (colonPos == (int)string::npos)
            return 0;
        return stoi(duration.substr(0, colonPos)) * 60 + stoi(duration.substr(colonPos + 1));
    }
};

// CLASS: Node  (circular doubly linked list)
class Node
{
public:
    Song song;
    Node *next;
    Node *prev;

    Node(Song s)
    {
        song = s;
        next = nullptr;
        prev = nullptr;
    }
};

// CLASS: CSVManager
class CSVManager
{
public:
    // Split a line into exactly fieldCount comma-separated tokens
    bool parseLine(string line, string fields[], int fieldCount)
    {
        int idx = 0;
        string token = "";
        for (int i = 0; i <= (int)line.length(); i++)
        {
            if (i == (int)line.length() || line[i] == ',')
            {
                if (idx >= fieldCount)
                    return false;
                fields[idx++] = token;
                token = "";
            }
            else
            {
                token += line[i];
            }
        }
        return idx == fieldCount;
    }

    void writeDefaultSongs()
    {
        ofstream file(CSV_FILE);
        if (!file.is_open())
        {
            cout << "Error: Cannot create CSV.\n";
            return;
        }
        file << EXPECTED_HEADER << "\n";

        // Songs by Toto
        file << "Africa,Toto,Rock,1982,4:55\n";
        file << "Rosanna,Toto,Rock,1982,5:31\n";
        file << "Hold the Line,Toto,Rock,1978,3:54\n";
        file << "I'll Be Over You,Toto,Rock,1986,5:31\n";
        file << "Lea,Toto,Rock,1986,5:31\n";

        // Songs by Bobby Caldwell
        file << "What You Won't Do for Love,Bobby Caldwell,R&B,1978,3:43\n";
        file << "Open Your Eyes,Bobby Caldwell,R&B,1982,4:12\n";
        file << "Coming Down from Love,Bobby Caldwell,R&B,1983,4:01\n";
        file << "I Give In,Bobby Caldwell,Smooth Jazz,1995,5:36\n";
        file << "Don't Ask Me Neighbor,Bobby Caldwell,Smooth Jazz,1995,4:50\n";
        file << "Until You Come Back to Me,Bobby Caldwell,Smooth Jazz,1995,4:23\n";

        // Songs by Michael Jackson
        file << "Billie Jean,Michael Jackson,Pop,1982,4:54\n";
        file << "Thriller,Michael Jackson,Pop,1982,5:57\n";
        file << "Beat It,Michael Jackson,Pop,1982,4:18\n";

        // Songs by Christopher Cross
        file << "Sailing, Christopher Cross,Adult Contemporary,1979,4:16\n";
        file << "Say You'll Be Mine,Christopher Cross,Adult Contemporary,1979,2:54\n";
        file << "Is There Something,Christopher Cross,Pop,1992,4:55\n";

        // Songs by Oasis
        file << "Wonderwall,Oasis,Indie Rock,1995,4:18\n";
        file << "Don't Look Back in Anger,Oasis,Indie Rock,1995,4:47\n";
        file << "Cast No Shadow,Oasis,Indie Rock,1995,4:54\n";
        file << "Champagne Supernova,Oasis,Indie Rock,1995,7:30\n";
        file << "Some Might Say,Oasis,Indie Rock,1995,5:27\n";
        file.close();
    }

    void createDefaultCSV()
    {
        ifstream checkFile(CSV_FILE);
        if (checkFile.is_open())
        {
            string header;
            getline(checkFile, header);
            checkFile.close();
            if (header == EXPECTED_HEADER)
                return;
            cout << "[!] Old CSV format detected. Recreating playlist.csv...\n";
        }
        writeDefaultSongs();
        cout << "Default playlist CSV created: " << CSV_FILE << "\n";
    }

    bool isDuplicate(string title, string artist)
    {
        return isDuplicateIn(CSV_FILE, title, artist);
    }

    bool isDuplicateIn(string filename, string title, string artist)
    {
        ifstream file(filename);
        if (!file.is_open())
            return false;
        string line;
        getline(file, line);
        while (getline(file, line))
        {
            if (line.empty())
                continue;
            string fields[5];
            if (!parseLine(line, fields, 5))
                continue;
            if (toLower(fields[0]) == toLower(title) &&
                toLower(fields[1]) == toLower(artist))
            {
                file.close();
                return true;
            }
        }
        file.close();
        return false;
    }

    void saveSong(Song s)
    {
        saveSongTo(CSV_FILE, s);
    }

    void saveSongTo(string filename, Song s)
    {
        ofstream file(filename, ios::app);
        if (!file.is_open())
        {
            cout << "Error: Cannot write CSV.\n";
            return;
        }
        file << s.title << "," << s.artist << "," << s.genre << ","
             << s.year << "," << s.duration << "\n";
        file.close();
    }

    int loadSongs(Song songs[], int maxSongs)
    {
        return loadSongsFrom(CSV_FILE, songs, maxSongs);
    }

    int loadSongsFrom(string filename, Song songs[], int maxSongs)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            cout << "Error: Cannot open CSV.\n";
            return 0;
        }
        string line;
        getline(file, line); // skip header
        int count = 0;
        while (getline(file, line) && count < maxSongs)
        {
            if (line.empty())
                continue;
            string fields[5];
            if (!parseLine(line, fields, 5))
                continue;
            songs[count++] = Song(fields[0], fields[1], fields[2],
                                  stoi(fields[3]), fields[4]);
        }
        file.close();
        return count;
    }
};

// CLASS: PlaylistManager
// Handles creation, saving, loading, and listing of named custom playlists.
// Each playlist is saved as "playlist_<name>.csv".
// A master index file "playlists.txt" tracks all playlist names.
class PlaylistManager
{
private:
    CSVManager csv;

    // Build the CSV filename for a given playlist name
    string buildFilename(string name)
    {
        return "playlist_" + name + ".csv";
    }

    // Register a playlist name in the index file (no duplicates)
    void registerPlaylist(string name)
    {
        // Check if already registered
        ifstream check(PLAYLIST_INDEX_FILE);
        if (check.is_open())
        {
            string line;
            while (getline(check, line))
            {
                if (toLower(line) == toLower(name))
                {
                    check.close();
                    return;
                }
            }
            check.close();
        }
        ofstream index(PLAYLIST_INDEX_FILE, ios::app);
        if (index.is_open())
        {
            index << name << "\n";
            index.close();
        }
    }

public:
    // Get all saved playlist names
    int getSavedPlaylists(string names[], int maxNames)
    {
        ifstream file(PLAYLIST_INDEX_FILE);
        if (!file.is_open())
            return 0;
        int count = 0;
        string line;
        while (getline(file, line) && count < maxNames)
        {
            if (!line.empty())
                names[count++] = line;
        }
        file.close();
        return count;
    }

    // Create a new empty playlist with a given name
    // Returns false if name is already taken
    bool createPlaylist(string name)
    {
        string filename = buildFilename(name);
        ifstream check(filename);
        if (check.is_open())
        {
            check.close();
            return false; // Already exists
        }
        ofstream file(filename);
        if (!file.is_open())
            return false;
        file << EXPECTED_HEADER << "\n";
        file.close();
        registerPlaylist(name);
        return true;
    }

    // Add a song to a named playlist
    void addSongToPlaylist(string name, Song s)
    {
        string filename = buildFilename(name);
        csv.saveSongTo(filename, s);
    }

    // Check for duplicate in a named playlist
    bool isDuplicateInPlaylist(string name, string title, string artist)
    {
        return csv.isDuplicateIn(buildFilename(name), title, artist);
    }

    // Load songs from a named playlist
    int loadPlaylist(string name, Song songs[], int maxSongs)
    {
        return csv.loadSongsFrom(buildFilename(name), songs, maxSongs);
    }

    // Display all songs in a named playlist
    void showPlaylist(string name)
    {
        Song songs[500];
        int count = loadPlaylist(name, songs, 500);
        if (count == 0)
        {
            cout << "  (Playlist is empty or not found.)\n";
            return;
        }
        cout << "  Playlist: \"" << name << "\"\n";
        cout << "  " << string(50, '-') << "\n";
        for (int i = 0; i < count; i++)
        {
            cout << "  " << (i + 1) << ". \""
                 << songs[i].title << "\" by " << songs[i].artist
                 << "  |  " << songs[i].genre
                 << "  |  " << songs[i].year
                 << "  |  " << songs[i].duration << "\n";
        }
        cout << "  " << string(50, '-') << "\n";
        cout << "  Total: " << count << " song(s)\n";
    }
};

// CLASS: MusicPlayer
class MusicPlayer
{
private:
    Node *head;
    Node *current;
    bool isPlaying; // true only after user picks a song to play
    CSVManager csv;
    PlaylistManager playlistMgr;
    string activePlaylistName; // "" = default playlist

    // Print full song details
    void displaySong(Song s)
    {
        cout << "  Title    : " << s.title << "\n";
        cout << "  Artist   : " << s.artist << "\n";
        cout << "  Genre    : " << s.genre << "\n";
        cout << "  Year     : " << s.year << "\n";
        cout << "  Duration : " << s.duration << "\n";
    }

    // Count all nodes
    int countSongs()
    {
        if (!head)
            return 0;
        int n = 0;
        Node *t = head;
        do
        {
            n++;
            t = t->next;
        } while (t != head);
        return n;
    }

    // Insert at end of circular list
    void insertAtEnd(Song s)
    {
        Node *newNode = new Node(s);
        if (!head)
        {
            newNode->next = newNode;
            newNode->prev = newNode;
            head = newNode;
        }
        else
        {
            Node *tail = head->prev;
            tail->next = newNode;
            newNode->prev = tail;
            newNode->next = head;
            head->prev = newNode;
        }
    }

    // Clear the linked list
    void clearList()
    {
        if (!head)
            return;
        Node *temp = head;
        Node *nextNode;
        head->prev->next = nullptr; // break the circle
        while (temp)
        {
            nextNode = temp->next;
            delete temp;
            temp = nextNode;
        }
        head = nullptr;
        current = nullptr;
        isPlaying = false;
    }

    // Fisher-Yates shuffle on the linked list
    void shuffleList()
    {
        int n = countSongs();
        if (n <= 1)
            return;

        Node *nodes[500];
        Node *t = head;
        for (int i = 0; i < n; i++)
        {
            nodes[i] = t;
            t = t->next;
        }

        for (int i = n - 1; i > 0; i--)
        {
            int j = rand() % (i + 1);
            Song tmp = nodes[i]->song;
            nodes[i]->song = nodes[j]->song;
            nodes[j]->song = tmp;
        }

        for (int i = 0; i < n; i++)
        {
            nodes[i]->next = nodes[(i + 1) % n];
            nodes[i]->prev = nodes[(i - 1 + n) % n];
        }

        head = nodes[0];
        current = head;
        isPlaying = false;
    }

    // Live playback timer
    // Counts up second by second with a progress bar.
    // On any keypress the timer stops immediately.
    void runTimer(Song s)
    {
        int total = s.durationInSeconds();
        int elapsed = 0;
        int barWidth = 30;

        cout << "\n  Press any key to stop...\n\n";

        while (elapsed <= total)
        {
            int eMin = elapsed / 60, eSec = elapsed % 60;
            int tMin = total / 60, tSec = total % 60;
            int filled = (total > 0) ? (elapsed * barWidth / total) : barWidth;

            cout << "\r  "
                 << (eMin < 10 ? "0" : "") << eMin << ":"
                 << (eSec < 10 ? "0" : "") << eSec
                 << "  [";
            for (int i = 0; i < barWidth; i++)
                cout << (i < filled ? '#' : '-');
            cout << "]  "
                 << (tMin < 10 ? "0" : "") << tMin << ":"
                 << (tSec < 10 ? "0" : "") << tSec
                 << "   " << flush;

            if (elapsed == total)
                break;

            // Check for keypress each 100ms for 10 iterations = 1 second
            bool stopped = false;
            for (int tick = 0; tick < 10; tick++)
            {
                if (_kbhit())
                {
                    _getch(); // consume the key
                    stopped = true;
                    break;
                }
                Sleep(100);
            }
            if (stopped)
            {
                cout << "\n\n  [STOPPED]\n";
                return;
            }
            elapsed++;
        }

        cout << "\n\n  [DONE] Finished playing.\n";
    }

    // ── Playlist Manager Submenu ──────────────────────────────────────────
    void managePlaylistsMenu()
    {
        int choice;
        do
        {
            clearScreen();
            cout << "=== Manage Playlists ===\n";
            if (activePlaylistName.empty())
                cout << "  Active: [Default Playlist]\n\n";
            else
                cout << "  Active: \"" << activePlaylistName << "\"\n\n";

            cout << "1. Create New Playlist\n";
            cout << "2. Show All Saved Playlists\n";
            cout << "3. Show Songs in Active Playlist\n";
            cout << "4. Switch to a Playlist\n";
            cout << "5. Add Song to a Playlist\n";
            cout << "0. Back to Main Menu\n";
            cout << "Enter your choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice)
            {
            case 1:
                createNewPlaylist();
                break;
            case 2:
                showAllPlaylists();
                break;
            case 3:
                showSongsInActivePlaylist();
                break;
            case 4:
                switchToPlaylist();
                break;
            case 5:
                addSongToNamedPlaylist();
                break;
            case 0:
                break;
            default:
                cout << "Invalid choice!\n";
            }

            if (choice != 0)
            {
                cout << "\nPress Enter to continue...";
                cin.get();
            }
        } while (choice != 0);
    }

    // Create a new named playlist, then immediately flow into adding songs
    void createNewPlaylist()
    {
        clearScreen();
        cout << "=== Create New Playlist ===\n\n";
        cout << "Enter a name for the new playlist: ";
        string name;
        getline(cin, name);

        if (name.empty())
        {
            cout << "[!] Playlist name cannot be empty.\n";
            return;
        }

        // Sanitize: replace spaces with underscores for safe filenames
        for (int i = 0; i < (int)name.length(); i++)
            if (name[i] == ' ')
                name[i] = '_';

        if (!playlistMgr.createPlaylist(name))
        {
            cout << "[!] A playlist named \"" << name << "\" already exists.\n";
            return;
        }

        cout << "[+] Playlist \"" << name << "\" created! (playlist_" << name << ".csv)\n";
        cout << "\nNow let's add songs to it.\n";
        cout << "Press Enter to continue...";
        cin.get();

        // Directly flow into adding songs to this new playlist
        addSongToNamedPlaylistByName(name);
    }

    // Show all saved playlists from the index
    void showAllPlaylists()
    {
        clearScreen();
        cout << "=== All Saved Playlists ===\n\n";
        string names[100];
        int count = playlistMgr.getSavedPlaylists(names, 100);
        if (count == 0)
        {
            cout << "  No custom playlists found.\n";
            cout << "  Only the default playlist (playlist.csv) exists.\n";
            return;
        }
        cout << "  0. [Default Playlist]  (playlist.csv)\n";
        for (int i = 0; i < count; i++)
            cout << "  " << (i + 1) << ". " << names[i]
                 << "  (playlist_" << names[i] << ".csv)\n";
        cout << "\n  Total custom playlists: " << count << "\n";
    }

    // Show songs in the currently active playlist (no prompt, just display)
    void showSongsInActivePlaylist()
    {
        clearScreen();
        cout << "=== Songs in Active Playlist ===\n\n";

        if (activePlaylistName.empty())
        {
            cout << "  Active: [Default Playlist]\n\n";
            if (!head)
            {
                cout << "  (Playlist is empty.)\n";
                return;
            }
            Node *temp = head;
            int num = 1;
            do
            {
                cout << "  " << num++ << ". \""
                     << temp->song.title << "\" by " << temp->song.artist
                     << "  |  " << temp->song.genre
                     << "  |  " << temp->song.year
                     << "  |  " << temp->song.duration << "\n";
                temp = temp->next;
            } while (temp != head);
            cout << "\n  Total: " << (num - 1) << " song(s)\n";
        }
        else
        {
            cout << "  Active: \"" << activePlaylistName << "\"\n\n";
            playlistMgr.showPlaylist(activePlaylistName);
        }
    }

    // Switch the active playlist (reloads the linked list)
    void switchToPlaylist()
    {
        clearScreen();
        cout << "=== Switch Playlist ===\n\n";
        string names[100];
        int count = playlistMgr.getSavedPlaylists(names, 100);

        cout << "  0. [Default Playlist]\n";
        for (int i = 0; i < count; i++)
            cout << "  " << (i + 1) << ". " << names[i] << "\n";

        cout << "\nEnter playlist name to switch to (or 'default'): ";
        string name;
        getline(cin, name);

        clearList();

        if (toLower(name) == "default" || name == "0" || name.empty())
        {
            activePlaylistName = "";
            Song songs[500];
            int c = csv.loadSongs(songs, 500);
            for (int i = 0; i < c; i++)
                insertAtEnd(songs[i]);
            cout << "\n[+] Switched to Default Playlist. Loaded " << c << " song(s).\n";
        }
        else
        {
            Song songs[500];
            int c = playlistMgr.loadPlaylist(name, songs, 500);
            if (c == 0)
            {
                cout << "\n[!] Playlist \"" << name << "\" not found or is empty.\n";
                // Fall back to default
                activePlaylistName = "";
                int d = csv.loadSongs(songs, 500);
                for (int i = 0; i < d; i++)
                    insertAtEnd(songs[i]);
            }
            else
            {
                activePlaylistName = name;
                for (int i = 0; i < c; i++)
                    insertAtEnd(songs[i]);
                cout << "\n[+] Switched to \"" << name << "\". Loaded " << c << " song(s).\n";
            }
        }
    }

    // Core logic: add songs from the master list to a named playlist by name.
    // Used both by createNewPlaylist() flow and addSongToNamedPlaylist().
    void addSongToNamedPlaylistByName(string playlistName)
    {
        while (true)
        {
            clearScreen();
            cout << "=== Add Song to \"" << playlistName << "\" ===\n\n";

            // Load all songs from the master CSV
            Song allSongs[500];
            int totalSongs = csv.loadSongs(allSongs, 500);

            if (totalSongs == 0)
            {
                cout << "  [!] No songs found in the master playlist.\n";
                return;
            }

            // Build list of songs NOT already in the target playlist
            Song available[500];
            int availCount = 0;
            for (int i = 0; i < totalSongs; i++)
            {
                if (!playlistMgr.isDuplicateInPlaylist(playlistName,
                                                       allSongs[i].title,
                                                       allSongs[i].artist))
                {
                    available[availCount++] = allSongs[i];
                }
            }

            if (availCount == 0)
            {
                cout << "  [!] All songs are already in playlist \"" << playlistName << "\".\n";
                return;
            }

            // Display available songs
            cout << "  Songs available to add:\n";
            cout << "  " << string(60, '-') << "\n";
            for (int i = 0; i < availCount; i++)
            {
                cout << "  " << (i + 1) << ". \""
                     << available[i].title << "\" by " << available[i].artist
                     << "  |  " << available[i].genre
                     << "  |  " << available[i].year
                     << "  |  " << available[i].duration << "\n";
            }
            cout << "  " << string(60, '-') << "\n";
            cout << "  0. Done\n\n";

            cout << "  Enter song number to add: ";
            int pick;
            cin >> pick;
            cin.ignore();

            if (pick == 0)
            {
                cout << "\n  [Done] Finished adding songs to \"" << playlistName << "\".\n";
                return;
            }

            if (pick < 1 || pick > availCount)
            {
                cout << "\n  [!] Invalid number. Please try again.\n";
                cout << "  Press Enter to continue...";
                cin.get();
                continue;
            }

            Song chosen = available[pick - 1];
            playlistMgr.addSongToPlaylist(playlistName, chosen);

            // If this playlist is currently active, also insert into linked list
            if (toLower(activePlaylistName) == toLower(playlistName))
            {
                insertAtEnd(chosen);
                cout << "\n  [+] \"" << chosen.title << "\" added! (Also added to the active player.)\n";
            }
            else
            {
                cout << "\n  [+] \"" << chosen.title << "\" added to \"" << playlistName << "\"!\n";
            }

            // Ask to add more or done
            cout << "  Add another song? (1 = Yes / 0 = Done): ";
            int more;
            cin >> more;
            cin.ignore();
            if (more != 1)
            {
                cout << "\n  [Done] Finished adding songs to \"" << playlistName << "\".\n";
                return;
            }
        }
    }

    // Add a song to a specific named playlist — pick playlist by number
    void addSongToNamedPlaylist()
    {
        clearScreen();
        cout << "=== Add Song to a Playlist ===\n\n";

        string names[100];
        int count = playlistMgr.getSavedPlaylists(names, 100);

        if (count == 0)
        {
            cout << "  No custom playlists found. Create one first!\n";
            return;
        }

        cout << "  Available playlists:\n";
        for (int i = 0; i < count; i++)
            cout << "  " << (i + 1) << ". " << names[i] << "\n";
        cout << "  0. Cancel\n\n";

        cout << "  Enter playlist number: ";
        int pick;
        cin >> pick;
        cin.ignore();

        if (pick == 0)
            return;

        if (pick < 1 || pick > count)
        {
            cout << "\n  [!] Invalid number.\n";
            return;
        }

        string chosenPlaylist = names[pick - 1];
        addSongToNamedPlaylistByName(chosenPlaylist);
    }

public:
    MusicPlayer()
    {
        head = nullptr;
        current = nullptr;
        isPlaying = false;
        activePlaylistName = "";
        csv.createDefaultCSV();
        loadFromCSV();
    }

    // Print active playlist label (used by main menu)
    void printActivePlaylist()
    {
        if (activePlaylistName.empty())
            cout << "[Default Playlist]";
        else
            cout << "\"" << activePlaylistName << "\"";
    }

    void loadFromCSV()
    {
        Song songs[500];
        int count = csv.loadSongs(songs, 500);
        for (int i = 0; i < count; i++)
            insertAtEnd(songs[i]);
    }

    // 1. Add Song
    void addSong()
    {
        clearScreen();
        cout << "=== Add Song ===\n\n";

        string title, artist, genre, duration;
        int year;

        cout << "Enter song title      : ";
        getline(cin, title);
        cout << "Enter artist name     : ";
        getline(cin, artist);

        if (csv.isDuplicate(title, artist))
        {
            cout << "\n[!] \"" << title << "\" by " << artist
                 << " is already in the playlist!\n"
                 << "    Duplicate was not added.\n";
            return;
        }

        cout << "Enter genre           : ";
        getline(cin, genre);
        cout << "Enter release year    : ";
        cin >> year;
        cin.ignore();
        cout << "Enter duration (MM:SS): ";
        getline(cin, duration);

        Song newSong(title, artist, genre, year, duration);
        insertAtEnd(newSong);
        csv.saveSong(newSong);

        cout << "\n[+] Song added successfully!\n\n";
        displaySong(newSong);
    }

    // 2. Play Current Song
    void playSong()
    {
        clearScreen();
        cout << "=== Now Playing ===\n\n";

        if (!head)
        {
            cout << "No songs in playlist!\n";
            return;
        }

        if (!isPlaying)
        {
            current = head;
            isPlaying = true;
        }

        displaySong(current->song);
        runTimer(current->song);
    }

    // 3. Next Song
    // Linked list showcase: follow ->next pointer.
    // If nothing is playing yet, start from head (no previous exists).
    void nextSong()
    {
        clearScreen();
        cout << "=== Next Song ===\n\n";

        if (!head)
        {
            cout << "No songs in playlist!\n";
            return;
        }

        if (!isPlaying)
        {
            current = head;
            isPlaying = true;
            cout << "  [No song was playing. Starting from the first song.]\n\n";
        }
        else
        {
            current = current->next; // Follow the ->next pointer
        }

        displaySong(current->song);

        cout << "\n  -- Linked List View --\n";
        cout << "  PREV : " << current->prev->song.title << "\n";
        cout << "  NOW  : " << current->song.title << "  <-- current\n";
        cout << "  NEXT : " << current->next->song.title << "\n\n";

        runTimer(current->song);
    }

    // 4. Previous Song
    // Linked list showcase: follow ->prev pointer.
    // If nothing is playing, block — there is no history yet.
    void prevSong()
    {
        clearScreen();
        cout << "=== Previous Song ===\n\n";

        if (!head)
        {
            cout << "No songs in playlist!\n";
            return;
        }

        if (!isPlaying)
        {
            cout << "  [!] No previous song — nothing has been played yet.\n";
            cout << "      Play or skip to a song first!\n";
            return;
        }

        current = current->prev; // Follow the ->prev pointer

        displaySong(current->song);

        cout << "\n  -- Linked List View --\n";
        cout << "  PREV : " << current->prev->song.title << "\n";
        cout << "  NOW  : " << current->song.title << "  <-- current\n";
        cout << "  NEXT : " << current->next->song.title << "\n\n";

        runTimer(current->song);
    }

    // 5. Show Playlist
    void showPlaylist()
    {
        clearScreen();
        cout << "=== Playlist ===\n\n";

        if (activePlaylistName.empty())
            cout << "  Active: [Default Playlist]\n\n";
        else
            cout << "  Active: \"" << activePlaylistName << "\"\n\n";

        if (!head)
        {
            cout << "Playlist is empty!\n";
            return;
        }

        Node *temp = head;
        int num = 1;
        do
        {
            cout << (isPlaying && temp == current ? " >> " : "    ");
            cout << num++ << ". \"" << temp->song.title
                 << "\" by " << temp->song.artist
                 << "  |  " << temp->song.genre
                 << "  |  " << temp->song.year
                 << "  |  " << temp->song.duration << "\n";
            temp = temp->next;
        } while (temp != head);

        if (!isPlaying)
            cout << "\n  (No song is currently selected.)\n";
    }

    // 6. Play Random Song
    void playRandom()
    {
        clearScreen();
        cout << "=== Random Song ===\n\n";

        if (!head)
        {
            cout << "Playlist is empty!\n";
            return;
        }

        int n = countSongs();
        int r = rand() % n;
        Node *temp = head;
        for (int i = 0; i < r; i++)
            temp = temp->next;

        current = temp;
        isPlaying = true;

        displaySong(current->song);
        runTimer(current->song);
    }

    // 7. Shuffle Playlist
    void shufflePlaylist()
    {
        clearScreen();
        cout << "=== Shuffle Playlist ===\n\n";

        if (!head)
        {
            cout << "Playlist is empty!\n";
            return;
        }

        shuffleList();

        cout << "  [+] Playlist shuffled! New order:\n\n";
        Node *temp = head;
        int num = 1;
        do
        {
            cout << "  " << num++ << ". \"" << temp->song.title
                 << "\" by " << temp->song.artist << "\n";
            temp = temp->next;
        } while (temp != head);

        cout << "\n  Now playing from the top...\n\n";
        isPlaying = true;
        displaySong(current->song);
        runTimer(current->song);
    }

    // 8. Manage Playlists
    void managePlaylists()
    {
        managePlaylistsMenu();
    }
};

// Main Menu
void menu()
{
    srand(time(0));
    MusicPlayer player;

    int choice;
    do
    {
        clearScreen();
        cout << "=== Music Player ===\n";
        cout << "  Active: ";
        player.printActivePlaylist();
        cout << "\n\n";
        cout << "1. Add Song\n";
        cout << "2. Play Current Song\n";
        cout << "3. Next Song\n";
        cout << "4. Previous Song\n";
        cout << "5. Show Playlist\n";
        cout << "6. Play Random Song\n";
        cout << "7. Shuffle Playlist\n";
        cout << "8. Manage Playlists\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
        case 1:
            player.addSong();
            break;
        case 2:
            player.playSong();
            break;
        case 3:
            player.nextSong();
            break;
        case 4:
            player.prevSong();
            break;
        case 5:
            player.showPlaylist();
            break;
        case 6:
            player.playRandom();
            break;
        case 7:
            player.shufflePlaylist();
            break;
        case 8:
            player.managePlaylists();
            break;
        case 0:
            clearScreen();
            cout << "Exiting Music Player. Goodbye!\n";
            break;
        default:
            clearScreen();
            cout << "Invalid choice! Please try again.\n";
        }

        if (choice != 0)
        {
            cout << "\nPress Enter to go back to menu...";
            cin.get();
        }

    } while (choice != 0);
}

int main()
{
    menu();
    return 0;
}
