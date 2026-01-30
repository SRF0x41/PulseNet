static bool loadAudioFile (const juce::File& file,
                           juce::AudioBuffer<float>& buffer,
                           double& sampleRate)
{
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats(); // WAV, AIFF, MP3, FLAC, etc.

    std::unique_ptr<juce::AudioFormatReader> reader (
        formatManager.createReaderFor (file));

    if (reader == nullptr)
        return false;

    sampleRate = reader->sampleRate;

    buffer.setSize ((int) reader->numChannels,
                    (int) reader->lengthInSamples);

    reader->read (&buffer,
                  0,
                  (int) reader->lengthInSamples,
                  0,
                  true,
                  true);

    return true;
}
