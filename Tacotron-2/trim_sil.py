import argparse
import librosa
import librosa.filters
import os


def trim_sil(in_wav_file, sr0, out_wav_file, trim_top_db, trim_fft_size, trim_hop_size):
    wav, sr = librosa.load(in_wav_file, sr=sr0)
    wav_trimmed, _ = librosa.effects.trim(wav, top_db=trim_top_db, frame_length=trim_fft_size,
                hop_length=trim_hop_size)
    librosa.output.write_wav(out_wav_file, wav_trimmed, sr)


def getListOfWavFiles(dirName):
    # create a list of file and sub directories
    # names in the given directory
    listOfFile = os.listdir(dirName)
    allFiles = list()
    allbasefiles = list()
    # Iterate over all the entries
    for entry in listOfFile:
        # Create full path
        if entry.endswith(".wav"):
            fullPath = os.path.join(dirName, entry)
            allFiles.append(fullPath)
            allbasefiles.append(entry)

    return allFiles, allbasefiles


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--in_wav_dir', default='')
    parser.add_argument('--in_sr', type=int, default=16000)
    parser.add_argument('--out_wav_dir', default='')
    args = parser.parse_args()

    print(args.in_wav_dir)
    print(args.out_wav_dir)

    trim_top_db = 40
    trim_fft_size = 256 # 512
    trim_hop_size = 64  # 128

    all_in_files, all_in_files_base = getListOfWavFiles(args.in_wav_dir)
    i = 0
    for in_file_full_path, in_file_base in zip(all_in_files, all_in_files_base):
        out_wav_full_Path = os.path.join(args.out_wav_dir, in_file_base)
        print(i)
        trim_sil(in_file_full_path, args.in_sr, out_wav_full_Path,
            trim_top_db, trim_fft_size, trim_hop_size)
        i += 1


if __name__ == '__main__':
    main()