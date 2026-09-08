CRON_CMD="0 0 * * * mkdir -p ~/FilesLog && ls -laS ~ > ~/FilesLog/sorted_files_\$(date +\%Y\%m\%d).txt"

if command -v crontab >/dev/null 2>&1; then
    (crontab -l 2>/dev/null | grep -F "FilesLog/sorted_files_" >/dev/null) || (crontab -l 2>/dev/null; echo "$CRON_CMD") | crontab -
else
    echo "Cron service is not available in this environment (Git Bash/Windows)."
fi